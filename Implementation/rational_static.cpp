#include "rational_static.h"
#include "uint_log2.h"

using namespace CFRN;
/********************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ********************************************************************/


///////////// Constructors ////////////////

template <unsigned char BYTE_COUNT>
T_rational_static<BYTE_COUNT>::T_rational_static() {
	bits_enc.set();
}

///////////// Utility Functions ///////////////

// ### TODO test options & decide on implementation ###
//*
template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::get_nth_digit_v1(const T_index digit, const T_uint& value) {
	return value & (((T_uint)1)<<digit);
}
template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::get_nth_digit_v2(const T_index digit, const T_uint& value) {
	return (value>>digit) % 2;
}
template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::get_nth_digit_v3(const T_index digit, const T_uint& value) {
	return (value>>digit) & 1;
}
//*/
template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::get_nth_digit(const T_index digit, const T_uint& value) {
	return get_nth_digit_v1(digit, value);
}

template <unsigned char BYTE_COUNT>
void T_rational_static<BYTE_COUNT>::set_nth_digit(const T_index digit, T_uint& value, bool bit) {
	if (bit) value |= (1<<digit);
	else value &= ~(1<<digit);
}

template <unsigned char BYTE_COUNT>
T_index T_rational_static<BYTE_COUNT>::log2(const T_uint& value) {
	return uint_log2<T_uint,T_index>(value);
}

/* intstream Functions
 *
 *
 *
 *
 *
 *
 *
 *
 */

template <unsigned char BYTE_COUNT>
T_rational_static<BYTE_COUNT>::T_intstream_static::T_intstream_static(T_rational_static<BYTE_COUNT>& rational)
: bstream(rational) {}

template <unsigned char BYTE_COUNT>
T_rational_static<BYTE_COUNT>::T_intstream_static::T_intstream_static(const T_intstream_static& uintstream)
: bstream(uintstream.bstream) {}
template <unsigned char BYTE_COUNT>
T_rational_static<BYTE_COUNT>::T_intstream_static::operator bool() const {
	return valid;
}

template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::T_intstream_static::skip_next() {
	if (!valid) return false;
	// Decode '1'
	if (!bstream.has_next()) return false;
	bool bool_tmp;
	bstream >> bool_tmp;
	if (bool_tmp == _0) {
		return true;
	}
	// Decode all other values
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
	return true;
}

template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::T_intstream_static::has_next() const {
	const typename bitstream::pos_data tmp = bstream.get_position_checkpoint();
	const bool ret = skip_next();
	bstream.restore_position(tmp);
	return ret;
}

template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::T_intstream_static::peek_next(T_int& value_dec) const {
	const typename bitstream::pos_data tmp = bstream.get_position_checkpoint();
	const bool ret = operator>>(value_dec);
	bstream.restore_position(tmp);
	return ret;
}

///////////// Encoding scheme for post-initial value in range [1, inf) ////////////////
//	Encoding	Decoded value
//	0			1
//	100			2
//	1010		3
//	1011		4
//	11000		5
//	11001		6
//	...
//
//	111...1 0* ####			=> (3+o)*2^n + int(###, n+o bits)
//	^-----^				n+1 = # of consecutive leading '1's
//	         ^			o   = bit after first '0' bit
//	           ^		### = following n+o bits
//
///////////////////////////////////////////////////////////////////////////////////////

// Returns whether or not value was successfully encoded into bitstream
template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::T_intstream_static& T_rational_static<BYTE_COUNT>::T_intstream_static::operator<<(const T_uint& value_dec) {
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
	const T_uint value_adj = value_dec+1;
	const T_index value_adj_log2 = (value_adj>1)
			? log2(value_adj) 
			: (8*sizeof(T_uint))+1;
	const bool value_adj_2nd_bit = get_nth_digit(value_adj_log2-1,value_adj);
	if (!bstream.has_next_multi((2*value_adj_log2)-(value_adj_2nd_bit?0:1))) return false;
		// ^ Test if enough bits are available
	bstream.set_multi((T_uint)(value_adj_2nd_bit?-4:-3),
					  value_adj_log2+(value_adj_2nd_bit ? 1:0));
	bstream.set_multi(value_adj, value_adj_log2-1);
	return true;
}

// Returns whether or not value was successfully decoded from bitstream
template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::T_intstream_static& T_rational_static<BYTE_COUNT>::T_intstream_static::operator>>(T_int& value_dec) {
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

///////////// Encoding scheme for initial value in (-inf, inf) ////////////////
//	Encoding	Decoded value
//	00			0
//	01			-1
//	10###		post_initial decoding(###)			in range [1,inf)
//	11###		-post_initial decoding(1###)		in range (-inf, -2]
//
///////////////////////////////////////////////////////////////////////////////
template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::T_intstream_static& T_rational_static<BYTE_COUNT>::T_intstream_static::reset_and_set_1st(const T_int& int_dec) {
	// (as this is meant only for the first entry, we assume the bitset has room for the two zero/sign bits)
	// Set to beginning of stream
	bstream.reset();
	// Categorize input
	const T_prefix bits_in = (int_dec==0?_ZERO: int_dec==-1?_NEG1: int_dec>0?_POS:_NEG);
	// Encode based on category
	switch (bits_in) {
		case _ZERO:
		case _NEG1:
			bstream.set_multi(bits_in, 2);
			break;
		case _POS:
			bstream.set_multi(bits_in, 2);
			operator<<(int_dec);
			break;
		case _NEG:
			bstream << (bool)MASK_NONZERO_RANGES;
			operator<<(-int_dec);
			break;
	}
}

template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::T_intstream_static& T_rational_static<BYTE_COUNT>::T_intstream_static::reset_and_get_1st(T_int& int_dec) {
	// (as this is meant only for the first entry, we assume the bitset has room for the two zero/sign bits)
	//Set to beginning of stream
	bstream.reset();
	// Get first bit
	bool bool_tmp;
	bstream >> bool_tmp;

	if (bool_tmp==MASK_NONZERO_RANGES) {
		bstream >> bool_tmp;
		int_dec = (bool_tmp==(bool)MASK_NEG? -1:0);
	} else {
		// Decode all other values
		T_uint_dec uint_tmp;
		operator>>(uint_tmp);
		if (uint_tmp==1) { //-> value > 0
			operator>>(uint_tmp);
			int_dec = uint_tmp;
		} else { //-> value < 0
			int_dec = -uint_tmp;
		}
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

template <unsigned char BYTE_COUNT>
T_rational_static<BYTE_COUNT>::bitstream::bitstream(T_rational_static& rational)
: bits_enc(rational.bits_enc), index(rational.bits_enc.size()) {}

template <unsigned char BYTE_COUNT>
T_rational_static<BYTE_COUNT>::bitstream::bitstream(const bitstream& bstream)
: bits_enc(bstream.bits_enc), index(bstream.index) {}

template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::bitstream& T_rational_static<BYTE_COUNT>::bitstream::operator=(const bitstream& bstream) {
	bits_enc = bstream.bits_enc;
	index = bstream.index;
	return *this;
}

template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::bitstream::has_next() const {
	return index >= 1;
}
template <unsigned char BYTE_COUNT>
bool T_rational_static<BYTE_COUNT>::bitstream::has_next_multi(T_rational_static<BYTE_COUNT>::T_index bitcount) const {
	return index >= bitcount;
}

//void T_rational_static<BYTE_COUNT>::bitstream::skip_next() {
//	++index;
//}
//void T_rational_static<BYTE_COUNT>::bitstream::skip_next_multi(T_rational_static<BYTE_COUNT>::T_index bitcount) {
//	index+=bitcount;
//}

template <unsigned char BYTE_COUNT>
void T_rational_static<BYTE_COUNT>::bitstream::reset() {
	index = bits_enc.size();
}

template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::bitstream& T_rational_static<BYTE_COUNT>::bitstream::operator>>(bool& bit) {
	bit = bits_enc[--index];
	return *this;
}
template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::bitstream& T_rational_static<BYTE_COUNT>::bitstream::operator<<(const bool bit) {
	bits_enc.set(--index, bit);
	return *this;
}
template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::bitstream& T_rational_static<BYTE_COUNT>::bitstream::get_multi(T_rational_static<BYTE_COUNT>::T_uint_dec& target,
														T_rational_static<BYTE_COUNT>::T_index bitcount) {
	index-=bitcount;
	const T_rational_static<BYTE_COUNT>::T_uint_dec neg_mask = (((T_rational_static<BYTE_COUNT>::T_uint_dec)-1)<<bitcount);
	target &= neg_mask;
	target = (bits_enc.to_ulong()>>index) & ~neg_mask;
	return *this;
}
template <unsigned char BYTE_COUNT>
typename T_rational_static<BYTE_COUNT>::bitstream& T_rational_static<BYTE_COUNT>::bitstream::set_multi(const T_rational_static<BYTE_COUNT>::T_uint_dec& source,
														T_rational_static<BYTE_COUNT>::T_index bitcount) {
	index-=bitcount;
	const T_rational_static<BYTE_COUNT>::T_uint_dec mask =	(~(((T_rational_static<BYTE_COUNT>::T_uint_dec)-1)<<bitcount))<<index;
	bits_enc = (std::bitset<8*(4)>)((bits_enc.to_ulong() & ~mask) | ((source<<index) & mask));
	return *this;
}
