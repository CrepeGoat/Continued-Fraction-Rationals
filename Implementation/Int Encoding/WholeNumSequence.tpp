
#include "BitTwiddles.h"
#include <stdexcept>
#include <iostream>


template <bool ENDIAN>
WholeNumSequence<ENDIAN>::WholeNumSequence() {}
template <bool ENDIAN>
WholeNumSequence<ENDIAN>::WholeNumSequence(BitSequence<ENDIAN,false> bits)
	: bseq(bits), track_prev{2,false} {}

template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::init(BitSequence<ENDIAN,false> bits) {
	bseq = bits;
	track_prev = {2,false};
}
template <bool ENDIAN>
BitSequence<ENDIAN,false> WholeNumSequence<ENDIAN>::get_bit_sequence() {
	return bseq;
}

template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::rho_leq_3d4() const {
	return *reinterpret_cast<const char*>(&track_prev) != 0; 
}
template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::update_rho(wnum_t n) {
	track_prev = {
			(unsigned int)((n > 2) + (n >= 2)), 
			track_prev.a_nm1 <= 1
		};
}


template <bool ENDIAN>
std::size_t WholeNumSequence<ENDIAN>::encoding_bitlength(wnum_t value) {
	// Excludes null cases
	if (value <= 0) return -1;
	// Handles small-value exception cases
	else if (value <= 1 + !rho_leq_3d4()) {
		return 1 + !rho_leq_3d4();
	}
	// Handles general cases
	value += rho_leq_3d4();
	//		Get significant variables
	std::size_t pos_msb = msb(value);
	const bool smsb = value & (pos_msb>>1);
	pos_msb = bit_pos_l2m(pos_msb);
	//		Check for sufficient bit storage
	return 2*pos_msb + smsb;
}


template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::has_next() const {
	BitSequence<ENDIAN,false> bseq_copy(bseq);
	const std::size_t len_1s_prefix = bseq_copy.get_streak(true);
	// Handles small-value exception cases
	if (len_1s_prefix == 0) {
		return bseq_copy.has_next(1 + !rho_leq_3d4());
	}
	// Handles general cases
	//		Preliminary check (ensures skip_next() will not fail)
	if (!bseq_copy.has_next(len_1s_prefix+1)) {
		return false;
	}
	//		Gets second-most-sig-bit
	bseq_copy.skip_next();
	//		Decisive check (informed by second-most-sig-bit)
	return !bseq_copy.peek_next() || bseq_copy.has_next(len_1s_prefix + 1);
}
template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::skip_next() {
	const std::size_t len_1s_prefix = bseq.get_streak(true);
	// Handles small-value exception cases
	if (len_1s_prefix == 0) {
		bseq.skip_next(1 + !rho_leq_3d4());
		return;
	}
	// Handles general cases
	//		Preliminary check (ensures skip_next() will not fail)
	if (!bseq.has_next(len_1s_prefix+1)) {
		bseq.skip_next(-1);
		return;
	}
	//		Performs full skip (cannot fail with insufficient space)
	bseq.skip_next();
	bseq.skip_next(len_1s_prefix + bseq.peek_next());
}
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::fits_next(const wnum_t& value) const {
	return bseq.has_next(encoding_bitlength(value));
}

template <bool ENDIAN>
inline bool WholeNumSequence<ENDIAN>::peek_next(wnum_t& value) const {
	return WholeNumSequence<ENDIAN>(*this).get_next(value);
}
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::get_next(wnum_t& value) {
	// Get significant variables
	const std::size_t len_1s_prefix = bseq.get_streak(true);
	// Make preliminary check
	if (!bseq.has_next(len_1s_prefix+1)) {
		bseq.skip_next(-1);
		return false;
	}
	bseq.skip_next(); // guaranteed 0-bit
	// Handles small-value exception cases
	if (len_1s_prefix == 0) {
		if (rho_leq_3d4()) {
			value = 1;
		} else {
			if (!bseq.has_next()) {
				bseq.skip_next(-1);
				return false;
			}
			value = 1 + bseq.get_next();
		}
	}
	// Handle general cases
	else {
		// Get other significant variables
		const bool smsb = !bseq.get_next();
		const std::size_t pos_smsb = len_1s_prefix - smsb;
		if (!bseq.has_next(pos_smsb)) {
			bseq.skip_next(-1);
			return false;
		}
		// Write bits to integer
		value = 0;
		bseq.get_to_int(value, pos_smsb);
		value += (wnum_t(2+smsb)<<(pos_smsb)) - rho_leq_3d4();
	}
	// Update rho & return
	update_rho(value);
	return true;
}

template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::set_next(wnum_t value) {
	// Handle null cases
	if (value <= 0) {
		throw std::domain_error("WholeNumSequence::set_next");
	}
	// Handle small-value exception cases
	if (value <= 1 + !rho_leq_3d4()) {
		// Check for sufficient bit storage
		if (!bseq.has_next(1 + !rho_leq_3d4())) {
			bseq.skip_next(-1); // skips to end of BitSequence
			return false;
		}
		// Set bits
		bseq << false;
		if (!rho_leq_3d4())
			bseq << (value >= 2);
	}
	// Handle general cases
	else {
		// Shifts value for general case
		value += rho_leq_3d4();	
		// Get significant variables
		std::size_t pos_msb = msb(value);
		const bool smsb = value & (pos_msb>>1);
		pos_msb = bit_pos_l2m(pos_msb);
		// Check for sufficient bit storage
		if (!bseq.has_next(2*pos_msb + smsb)) {
			bseq.skip_next(-1); // skips to end of BitSequence
			return false;
		}
		// Set bits
		bseq.set_streak(true, pos_msb - !smsb);
		bseq << false << !smsb;
		bseq.set_from_int(value, pos_msb-1);
		// Reverse any value shift
		value -= rho_leq_3d4();
	}
	// Update rho & return
	update_rho(value);
	return true;
}

template <bool ENDIAN>
inline WholeNumSequence<ENDIAN>& WholeNumSequence<ENDIAN>::operator>>(wnum_t& value) {
	if (!get_next(value)) {
		throw std::range_error("WholeNumSequence::operator>> - incomplete entry stored");
	}
	return *this;
}
template <bool ENDIAN>
inline WholeNumSequence<ENDIAN>& WholeNumSequence<ENDIAN>::operator<<(const wnum_t& value) {
	if (!set_next(value)) {
		throw std::range_error("WholeNumSequence::operator<< - insufficient space for argument");
	}
	return *this;
}
