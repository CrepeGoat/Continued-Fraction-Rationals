#include "rational.h"

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
	if(!value) return -1;
	const float FloatValue = value;
	return ((((*(unsigned long *)&FloatValue) & 0x7f800000) >> 23) - 127);
}


/*
inline T_rational::T_index get_no_of_encoding_bits_scheme1(
		const T_rational::T_uint_dec& value) {
	switch (value-1) {
		0: return 5; break;
		1: return 1; break;
		2: return 3; break;
		3: 4: return 4; break;
		5: 6: 7: return 5; break;
		default:
			T_rational::T_uint_dec value_temp=value_dec-1;
			T_rational::T_index bits_cntr=1;
			while (value_temp>4) {
				value_temp>>=1;
				bits_cntr+=2;
			};
			if (value_temp==2) {
				++bits_cntr;
			} //else if (value_temp==3) {}
			return bits_cntr;
	}
}

template <T_index N>
static void T_rational::encode_into_bitstream_scheme1(
		const T_uint_dec& value_dec,
		std::bitset<N>& bits_enc, T_index& i_enc) {
	// '0' is an invalid input -> treat '0' as 'MAX+1' or 'CTRL'
	
	// 1ST BIT
	if (value_dec==1) {
		bits_enc.set(i_enc++, _0);
		return;
	} else { // if (value_dec >=2)
		bits_enc.set(i_enc++, _1);
	}
	// 2ND BIT
	if (value_dec<=4) {
		// '0' VALUE
		if (value_dec==0) {
			bits_enc.set(i_enc++, _1);
			bits_enc.set(i_enc++, _0);
			bits_enc.set(i_enc++, _0);
			bits_enc.set(i_enc++, _0);
			return;
		}	
		bits_enc.set(i_enc++, _0);
		// 3RD BIT
		if (value_dec==2) {
			bits_enc.set(i_enc++, _0);
		} else {
			bits_enc.set(i_enc++, _1);
			// 4TH BIT
			bits_enc.set(i_enc++, !get_nth_digit(0,value_dec));
		}
		return;
	} else {// if (value_dec >=5)
		bits_enc.set(i_enc++, _1);
	}
	// 3RD BIT
	if (value_dec<=7) {
		bits_enc.set(i_enc++, _0);
		// 4TH-5TH BITS
		bits_enc.set(i_enc++, get_nth_digit(1,value_dec));
		bits_enc.set(i_enc++, get_nth_digit(0,value_dec));
		return;
	} else {//if (value_dec >= 8)
		bits_enc.set(i_enc++, _1);
	}
	// 4TH-NTH BITs
	// ####TODO####
	T_uint_dec value_temp = value_dec-2;
	const T_index n = log2(value_temp)-2;
	const unsigned char o = (n >
	
}
*/

// ### TODO test speed ###
/*
T_rational::T_index T_rational::no_of_bits_in_encoding_of(const T_uint_dec& value) {
	if (value == 1) return 1;
	else if (value==0 || value == -1) return (16*sizeof(T_uint_dec));

	const T_uint_dec value_adj = value+1;
	const T_index value_adj_log2 = log2(value_adj);
	const T_index value_adj_2nd_bit = 
			get_nth_digit(value_adj_log2-1,value_adj) ? 1:0;
	return (value_adj_log2<<1)|value_adj_2nd_bit;
}

T_rational::T_uint_dec T_rational::max_value_for_bitcount_of(
		T_rational::T_index bitcount) {
	if (bitcount==1 || bitcount==2)
		return 1;
	const T_index n_enc = (bitcount-3)/2, o_enc = (bitcount-3)%2;
	return (((T_uint_dec)2+o_enc)<<(n_enc+1))-2; 
}

bool T_rational::can_encode_value_in_bitset_v1(const T_uint_dec& value_dec,
													 T_index& i_enc) const {
	return no_of_bits_in_encoding_of(value_dec) <= bits_enc.size()-i_enc;
}
bool T_rational::can_encode_value_in_bitset_v2(const T_uint_dec& value_dec,
													 T_index& i_enc) const {
	return (i_enc<bits_enc.size()) && (max_value_for_bitcount_of(bits_enc.size()-i_enc) >= value_dec);
}
bool T_rational::can_encode_value_in_bitset(const T_uint_dec& value_dec,
												  T_index& i_enc) const {
	return can_encode_value_in_bitset_v2(value_dec, i_enc);
}
*/
// Returns whether or not value was successfully encoded into bitstream
bool T_rational::encode_into_bitstream(
		const T_uint_dec& value_dec, T_index& i_enc) {
	// '0' is an invalid input -> treat '0' as 'MAX+1'
	// Encode 1
	if (i_enc >= bits_enc.size()) return false;
		// ^ Test if enough bits are available
	if (value_dec==1) {
		bits_enc.set(i_enc++, _0);
		return true;
	} else { // if (value_dec >=2)
		bits_enc.set(i_enc++, _1);
	}
	// Encode all other values
	const T_uint_dec value_adj = value_dec+1;
	const T_index value_adj_log2 = (value_adj>1)
			? log2(value_adj) 
			: (8*sizeof(T_uint_dec))|1;
	const bool value_adj_2nd_bit = get_nth_digit(value_adj_log2-1,value_adj);
	if (i_enc +((value_adj_log2<<1)|(value_adj_2nd_bit?1:0))-2 >= bits_enc.size()) return false;
		// ^ Test if enough bits are available
	T_index i;
	for (i=(value_adj_2nd_bit ? 1:2); i<value_adj_log2; ++i) {
		bits_enc.set(i_enc++, _1);
	}
	bits_enc.set(i_enc++, _0);
	bits_enc.set(i_enc++, !value_adj_2nd_bit);
	for (i=value_adj_log2-2; i!=((T_index)-1); --i) {
		bits_enc.set(i_enc++, get_nth_digit(i,value_adj));
	}
	return true;
}

bool T_rational::decode_from_bitstream(T_index& i_enc,T_uint_dec& value_dec) const {
	// Decode '1'
	if (i_enc >= bits_enc.size()) return false;
	if (bits_enc[i_enc++] == _0) {
		value_dec = 1;
		return true;
	}
	// Decode all other values
	value_dec = 0; // clear return value
	//		Count leading '1's
	const T_index i_enc_preinc = i_enc;
	do {
		if (((2*i_enc)-i_enc_preinc)+1 >= bits_enc.size()) return false;
			// checks if bits exist to encode minimum-length value
			// ^ assumes bits_enc[i_enc] == _0, bits_enc[i_enc+1] == _0
	} while (bits_enc[i_enc++] == _1);
	const T_index enc_1streak = i_enc-i_enc_preinc-1;
	//		Get bit after first '0'
	const bool enc_2nd_digit = bits_enc[i_enc++];
	if (enc_2nd_digit && i_enc +enc_1streak >= bits_enc.size())
		return false;
	//		Get remaining bits
	for (T_index i=enc_1streak+(enc_2nd_digit ? 1:0)-1; i!=((T_index)-1); --i) {
		set_nth_digit(i, value_dec, bits_enc[i_enc++]);
	}
	value_dec+=((enc_2nd_digit ? 4:3)<<enc_1streak);
	--value_dec;
	return true;
}

bool T_rational::encode_1st_into_bitstream(const T_int_dec& value_dec, T_index& i_enc) {
	// (as this is meant only for the first entry, we assume:
	//		1. the index starts at i_enc == 0
	//		2. the bitset has room for the two zero/sign bits)
	// Encode 0
	if (value_dec==0) {
		bits_enc.set(0, _0);
		i_enc = 1;
		return true;
	} else { // if (value_dec != 0)
		bits_enc.set(0, _1);
		// Encode signs
		bits_enc.set(1, value_dec>0?_0:_1);
		i_enc = 2;
		return encode_into_bitstream(value_dec>0?value_dec:-value_dec, i_enc);
	}
}

bool T_rational::decode_1st_from_bitstream(T_index& i_enc,T_int_dec& value_dec) const {
	// (as this is meant only for the first entry, we assume:
	//		1. the index starts at i_enc == 0
	//		2. the bitset has room for the two zero/sign bits)
	// Decode '0'
	if (bits_enc[0]==_0) {
		value_dec = 0;
		i_enc = 1;
		return true;
	}
	// Decode all other values
	i_enc = 2;
	T_uint_dec result_pos;
	if (!decode_from_bitstream(i_enc, result_pos)) {
		return false;
	}
	value_dec = result_pos * (bits_enc[1]==_0 ?1:-1);
	return true;
}
