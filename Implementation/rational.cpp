#include "rational.h"
#include "uint_log2.h"
///////////// Constructors ////////////////

T_rational::T_rational() {
	bits_enc.set();
}

///////////// Utility Functions ///////////////

// ### TODO test options & decide on implementation ###
//*
bool T_rational::get_nth_digit_v1(const T_index digit, const T_uint_dec& value) {
	return value & (((T_uint_dec)1)<<digit);
}
bool T_rational::get_nth_digit_v2(const T_index digit, const T_uint_dec& value) {
	return (value>>digit) % 2;
}
bool T_rational::get_nth_digit_v3(const T_index digit, const T_uint_dec& value) {
	return (value>>digit) & 1;
}
//*/
bool T_rational::get_nth_digit(const T_index digit, const T_uint_dec& value) {
	return get_nth_digit_v1(digit, value);
}

void T_rational::set_nth_digit(const T_index digit, T_uint_dec& value, bool bit) {
	if (bit) value |= (1<<digit);
	else value &= ~(1<<digit);
}

T_rational::T_index T_rational::log2(const T_uint_dec& value) {
	return uint_log2<T_uint_dec,T_index>(value);
}

// Returns whether or not value was successfully encoded into bitstream
bool T_rational::encode_into_bitstream(const T_uint_dec& value_dec, bitstream& bstream) {
	// '0' is an invalid input -> treat '0' as 'MAX+1'
	// Encode 1
	if (!bstream.has_next()) return false;
		// ^ Test if enough bits are available
	if (value_dec==1) {
		bstream << _0;
		return true;
	} else { // if (value_dec >=2)
		bstream << _1;
	}
	// Encode all other values
	const T_uint_dec value_adj = value_dec+1;
	const T_index value_adj_log2 = (value_adj>1)
			? log2(value_adj) 
			: (8*sizeof(T_uint_dec))+1;
	const bool value_adj_2nd_bit = get_nth_digit(value_adj_log2-1,value_adj);
	if (!bstream.has_next_multi((2*value_adj_log2)-(value_adj_2nd_bit?0:1))) return false;
		// ^ Test if enough bits are available
	bstream.set_multi((T_uint_dec)(value_adj_2nd_bit?-4:-3),
					  value_adj_log2+(value_adj_2nd_bit ? 1:0));
	bstream.set_multi(value_adj, value_adj_log2-1);
	return true;
}

// Returns whether or not value was successfully decoded from bitstream
bool T_rational::decode_from_bitstream(bitstream& bstream, T_uint_dec& value_dec) const {
	// Decode '1'
	if (!bstream.has_next()) return false;
	bool bool_tmp;
	bstream >> bool_tmp;
	if (bool_tmp == _0) {
		value_dec = 1;
		return true;
	}
	// Decode all other values
	value_dec = 0; // clear return value
	//		Count leading '1's
	T_index enc_1streak = 0;
	while (true) {
		if (!bstream.has_next_multi(enc_1streak+2)) return false;
			// checks if bits exist to encode minimum-length value
			// i.e., ^ assumes next two bits are _0, _0
		bstream >> bool_tmp;
		if (bool_tmp != _1) break;
		++enc_1streak;
	}
	//const T_index enc_1streak = i_enc-i_enc_preinc-1;
	//		Get bit after first '0'
	bstream >> bool_tmp;
	const bool enc_2nd_digit = bool_tmp;
	if (enc_2nd_digit && !bstream.has_next_multi(enc_1streak+1))
		return false;
	//		Get remaining bits
	bstream.get_multi(value_dec, enc_1streak+(enc_2nd_digit ? 1:0));
	value_dec+=((enc_2nd_digit ? 4:3)<<enc_1streak);
	--value_dec;
	return true;
}

void T_rational::encode_1st_into_bitstream(const T_prefix prefix, bitstream& bstream) {
	// (as this is meant only for the first entry, we assume:
	//		1. the index starts at i_enc == 0
	//		2. the bitset has room for the two zero/sign bits)
	bstream = bitstream(*this);
	// Encode 0
	if (prefix&_ZERO) {
		bstream << _0;
	} else { // if (value != 0)
		// Encode signs
		bstream.set_multi(prefix&_POS_NEG?2:3, 2);
	}
}

void T_rational::decode_1st_from_bitstream(bitstream& bstream, T_prefix& prefix) const {
	// (as this is meant only for the first entry, we assume:
	//		1. the index starts at i_enc == 0
	//		2. the bitset has room for the two zero/sign bits)
	bool bool_tmp;
	// Decode '0'
	bstream >> bool_tmp;
	if (bool_tmp==_0) {
		prefix = _ZERO;
	} else {
		// Decode all other values
		bstream >> bool_tmp;
		prefix = (bool_tmp==_0 ?_POS_NEG:0);
	}
}


/* Bitstream Functions
 *
 *
 *
 *
 *
 *
 *
 *
 */

T_rational::bitstream::bitstream(T_rational& rational)
: bits_enc(rational.bits_enc), index(rational.bits_enc.size()) {}

T_rational::bitstream::bitstream(std::bitset<8*(4)>& bits)
: bits_enc(bits), index(bits.size()) {}

T_rational::bitstream::bitstream(const bitstream& bstream)
: bits_enc(bstream.bits_enc), index(bstream.index) {}

T_rational::bitstream& T_rational::bitstream::operator=(const bitstream& bstream) {
	bits_enc = bstream.bits_enc;
	index = bstream.index;
	return *this;
}

bool T_rational::bitstream::has_next() const {
	return index >= 1;
}
bool T_rational::bitstream::has_next_multi(T_rational::T_index bitcount) const {
	return index >= bitcount;
}

//void T_rational::bitstream::skip_next() {
//	++index;
//}
//void T_rational::bitstream::skip_next_multi(T_rational::T_index bitcount) {
//	index+=bitcount;
//}

void T_rational::bitstream::reset() {
	index = bits_enc.size();
}

T_rational::bitstream& T_rational::bitstream::operator>>(bool& bit) {
	bit = bits_enc[--index];
	return *this;
}
T_rational::bitstream& T_rational::bitstream::operator<<(const bool bit) {
	bits_enc.set(--index, bit);
	return *this;
}
T_rational::bitstream& T_rational::bitstream::get_multi(T_rational::T_uint_dec& target,
														T_rational::T_index bitcount) {
	index-=bitcount;
	const T_rational::T_uint_dec neg_mask = (((T_rational::T_uint_dec)-1)<<bitcount);
	target &= neg_mask;
	target = (bits_enc.to_ulong()>>index) & ~neg_mask;
	return *this;
}
T_rational::bitstream& T_rational::bitstream::set_multi(const T_rational::T_uint_dec& source,
														T_rational::T_index bitcount) {
	index-=bitcount;
	const T_rational::T_uint_dec mask =	(~(((T_rational::T_uint_dec)-1)<<bitcount))<<index;
	bits_enc = (std::bitset<8*(4)>)((bits_enc.to_ulong() & ~mask) | ((source<<index) & mask));
	return *this;
}
