
#include "BitTwiddles.hpp"
#include <stdexcept>

template <bool ENDIAN>
std::size_t WholeNumSeqSBS2<ENDIAN>::encoding_bitlength(uintmax_t value) {
	// Excludes null cases
	if (value <= 0) return -1;
	// Handles small-value exception cases
	else if (value <= 2) {
		return 2;
	}
	// Handles general cases
	//		Get significant variables
	std::size_t pos_msb = msb(value);
	const bool smsb = value & (pos_msb>>1);
	pos_msb = bit_pos_0h(pos_msb);
	//		Check for sufficient bit storage
	return 2*pos_msb + smsb;
}


template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::has_next() const {
	BitSequence<ENDIAN,false> bseq_copy(this->bseq);
	const std::size_t len_1s_prefix = bseq_copy.read_streak(true);
	// Handles small-value exception cases
	if (len_1s_prefix == 0) {
		return bseq_copy.has_next(2);
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
void WholeNumSeqSBS2<ENDIAN>::skip_next() {
	const std::size_t len_1s_prefix = this->bseq.read_streak(true);
	// Handles small-value exception cases
	if (len_1s_prefix == 0) {
		this->bseq.skip_next(2);
		return;
	}
	// Handles general cases
	//		Preliminary check (ensures skip_next() will not fail)
	if (!this->bseq.has_next(len_1s_prefix+1)) {
		this->bseq.skip_next(-1);
		return;
	}
	//		Performs full skip (cannot fail with insufficient space)
	this->bseq.skip_next();
	this->bseq.skip_next(len_1s_prefix + this->bseq.peek_next());
}
template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::fits_next(const uintmax_t& value) const {
	return this->bseq.has_next(encoding_bitlength(value));
}

template <bool ENDIAN>
inline bool WholeNumSeqSBS2<ENDIAN>::peek_next(uintmax_t& value) const {
	return WholeNumSeqSBS2<ENDIAN>(*this).read_next(value);
}
template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::read_next(uintmax_t& value) {
	// Get significant variables
	const std::size_t len_1s_prefix = this->bseq.read_streak(true);
	// Make preliminary check
	if (!this->bseq.has_next(len_1s_prefix+1)) {
		this->bseq.skip_next(-1);
		return false;
	}
	this->bseq.skip_next(); // guaranteed 0-bit
	// Handles small-value exception cases
	if (len_1s_prefix == 0) {
		if (!this->bseq.has_next()) {
			this->bseq.skip_next(-1);
			return false;
		}
		value = 1 + this->bseq.read_next();
	}
	// Handle general cases
	else {
		// Get other significant variables
		const bool smsb = !this->bseq.read_next();
		const std::size_t pos_smsb = len_1s_prefix - smsb;
		if (!this->bseq.has_next(pos_smsb)) {
			this->bseq.skip_next(-1);
			return false;
		}
		// Write bits to integer
		value = 0;
		this->bseq.read_to_int(value, pos_smsb);
		value += (uintmax_t(2+smsb)<<(pos_smsb));
	}
	return true;
}

template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::write_next(uintmax_t value) {
	// Handle null cases
	if (value <= 0) {
		throw std::domain_error("WholeNumSeqSBS2::write_next");
	}

	// Handle small-value exception cases
	if (value <= 2) {
		// Check for sufficient bit storage
		if (!this->bseq.has_next(2)) {
			this->bseq.skip_next(-1); // skips to end of BitSequence
			return false;
		}
		// Set bits
		this->bseq << false;
		this->bseq << (value >= 2);
	}
	
	// Handle general cases
	else {
		// Get significant variables
		std::size_t pos_msb = msb(value);
		const bool smsb = value & (pos_msb>>1);
		pos_msb = bit_pos_0h(pos_msb);
		// Check for sufficient bit storage
		if (!this->bseq.has_next(2*pos_msb + smsb)) {
			this->bseq.skip_next(-1); // skips to end of BitSequence
			return false;
		}
		// Set bits
		this->bseq.write_streak(true, pos_msb - !smsb);
		this->bseq << false << !smsb;
		this->bseq.write_from_int(value, pos_msb-1);
		// Reverse any value shift
	}
	return true;
}

template <bool ENDIAN>
inline WholeNumSeqSBS2<ENDIAN>& WholeNumSeqSBS2<ENDIAN>::operator>>(uintmax_t& value) {
	if (!read_next(value)) {
		throw std::range_error("WholeNumSeqSBS2::operator>> - incomplete entry stored");
	}
	return *this;
}
template <bool ENDIAN>
inline WholeNumSeqSBS2<ENDIAN>& WholeNumSeqSBS2<ENDIAN>::operator<<(const uintmax_t& value) {
	if (!write_next(value)) {
		throw std::range_error("WholeNumSeqSBS2::operator<< - insufficient space for argument");
	}
	return *this;
}