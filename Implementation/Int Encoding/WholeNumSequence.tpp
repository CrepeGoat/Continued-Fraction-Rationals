
#include "BitTwiddles.h"
#include <stdexcept>


template <bool ENDIAN>
WholeNumSequence<ENDIAN>::WholeNumSequence() {}
template <bool ENDIAN>
WholeNumSequence<ENDIAN>::WholeNumSequence(BitSequence<ENDIAN,false> bits)
	: bseq(bits) {}

template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::init(BitSequence<ENDIAN,false> bits) {
	bseq = bits;
}
template <bool ENDIAN>
BitSequence<ENDIAN,false> WholeNumSequence<ENDIAN>::get_bit_sequence() {
	return bseq;
}


template <bool ENDIAN>
std::size_t WholeNumSequence<ENDIAN>::encoding_bitlength(wnum_t value) {
	value += 1;
	// Get significant variables
	std::size_t pos_msb = msb(value);
	const bool smsb = value & (pos_msb>>1);
	pos_msb = bit_pos_0h(pos_msb);
	// Check for sufficient bit storage
	return 2*pos_msb + smsb - (value == 2);
}


template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::has_next() const {
	BitSequence<ENDIAN,false> bseq_copy(bseq);
	const std::size_t len_ones_prefix = bseq_copy.get_streak(true);
	if (!bseq_copy.has_next(len_ones_prefix+1)) {
		return false;
	}
	if (len_ones_prefix == 0) {
		return true;
	}
	bseq_copy.skip_next();
	return bseq_copy.has_next(len_ones_prefix + bseq_copy.peek_next());
}
template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::skip_next() {
	const std::size_t len_ones_prefix = bseq.get_streak(true);
	if (!bseq.has_next(len_ones_prefix+1)) {
		bseq.skip_next(-1);
		return;
	}
	bseq.skip_next();
	if (len_ones_prefix != 0) {
		bseq.skip_next(len_ones_prefix + bseq.peek_next());
	}
}
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::fits_next(const wnum_t& value) const {
	return bseq.has_next(encoding_bitlength(value));
}

template <bool ENDIAN>
inline bool WholeNumSequence<ENDIAN>::peek_next(wnum_t& value) const {
	return WholeNumSequence<ENDIAN>(bseq).get_next(value);
}
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::get_next(wnum_t& value) {
	// Get significant variables
	const std::size_t len_ones_prefix = bseq.get_streak(true);
	if (!bseq.has_next(len_ones_prefix+1)) {
		bseq.skip_next(-1);
		return false;
	}
	bseq.skip_next();
	if (len_ones_prefix == 0) {
		value = 1;
		return true;
	}
	const bool smsb = !bseq.get_next();
	const std::size_t pos_msb_m1 = len_ones_prefix - smsb;
	if (!bseq.has_next(pos_msb_m1)) {
		bseq.skip_next(-1);
		return false;
	}
	// Write bits to integer
	value = 0;
	bseq.get_to_int(value, pos_msb_m1);
	value += (wnum_t(2+smsb)<<(pos_msb_m1)) - 1;
	return true;
}

template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::set_next(wnum_t value) {
	if (value == 0) {
		throw std::domain_error("WholeNumSequence::set_next");
	}
	value += 1;
	// Get significant variables
	std::size_t pos_msb = msb(value);
	const bool smsb = value & (pos_msb>>1);
	pos_msb = bit_pos_0h(pos_msb);
	// Check for sufficient bit storage
	if (!bseq.has_next(2*pos_msb + smsb - (value == 2))) {
		bseq.skip_next(-1); // skips to end of BitSequence
		return false;
	} else {
		// Set bits
		bseq.set_streak(true, pos_msb - !smsb);
		bseq << false;
		if (value > 2) {
			bseq << !smsb;
			bseq.set_from_int(value, pos_msb-1);
		}
		return true;
	}
}

template <bool ENDIAN>
inline WholeNumSequence<ENDIAN>& WholeNumSequence<ENDIAN>::operator>>(wnum_t& value) {
	if (!get_next(value)) {
		throw std::range_error("WholeNumSequence::operator>>");
	}
	return *this;
}
template <bool ENDIAN>
inline WholeNumSequence<ENDIAN>& WholeNumSequence<ENDIAN>::operator<<(const wnum_t& value) {
	if (!set_next(value)) {
		throw std::range_error("WholeNumSequence::operator<<");
	}
	return *this;
}
