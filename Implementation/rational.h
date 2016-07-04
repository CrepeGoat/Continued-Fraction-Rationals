#include <bitset>

#ifndef RATIONAL_H
#define RATIONAL_H

/* TODO List:
 *		- make/use bitstream class for template-adjustable implementation
 *		- consider sizeless rational number class
 *
 */

class T_rational {
//private:
public:
	
	// Typedefs/Const Values
	typedef unsigned long int T_uint_dec;
	typedef long int T_int_dec;
	typedef unsigned char T_index;
	
	static const bool _0 = false;
	static const bool _1 = true;
	
	typedef unsigned char T_prefix;
	static const T_prefix _ZERO = 1;
	static const T_prefix _POS_NEG = 2;
	
	
	// Internal Variables
	std::bitset<8*(4)> bits_enc;
	
	class bitstream;
	
	// Support Functions
	////////// TODO test //////////////
	static bool get_nth_digit_v1(const T_index digit, const T_uint_dec& value);
	static bool get_nth_digit_v2(const T_index digit, const T_uint_dec& value);
	static bool get_nth_digit_v3(const T_index digit, const T_uint_dec& value);
	static inline bool get_nth_digit(const T_index digit, const T_uint_dec& value);
	static inline void set_nth_digit(const T_index digit, T_uint_dec& value, bool bit);
	static T_index log2_v1(const T_uint_dec& x);
	static T_index log2_v2(const T_uint_dec& x);
	static inline T_index log2(const T_uint_dec& x);
	
	/*
	static inline T_rational::T_index no_of_bits_in_encoding_of(const T_uint_dec& value);
	static inline T_rational::T_uint_dec max_value_for_bitcount_of(T_index bitcount);
	inline bool can_encode_value_in_bitset_v1(const T_uint_dec& value_dec, T_index& i_enc) const;
	inline bool can_encode_value_in_bitset_v2(const T_uint_dec& value_dec, T_index& i_enc) const;
	inline bool can_encode_value_in_bitset(const T_uint_dec& value_dec, T_index& i_enc) const;
	*/
	
	
	// Utility Functions
	bool encode_into_bitstream(const T_uint_dec& value_dec, bitstream& bstream);
	bool decode_from_bitstream(bitstream& bstream, T_uint_dec& value_dec) const;
	void encode_1st_into_bitstream(const T_prefix prefix, bitstream& bstream);
	void decode_1st_from_bitstream(bitstream& bstream, T_prefix& prefix) const;
	
public:
	// Constructors
	T_rational();
	//template <typename P_INT>
	//t_float_contfrac(P_INT p_begin, P_INT p_end);
	
	// Float Casting
	//operator float();
};

class T_rational::bitstream {
//private:
public:
	std::bitset<8*(4)>& bits_enc;
	T_index index;
	
	static T_rational::T_uint_dec mask_v1(T_rational::T_index first_index, T_rational::T_index length);
	static T_rational::T_uint_dec mask_v2(T_rational::T_index first_index, T_rational::T_index length);
	static T_rational::T_uint_dec mask(T_rational::T_index first_index, T_rational::T_index length);
public:
	bitstream(T_rational& rational);
	bitstream(std::bitset<8*(4)>& bits);
	bitstream(const bitstream& bstream);
	inline bitstream& operator=(const bitstream& bstream);
	
	
	bool has_next() const;
	bool has_next_multi(T_rational::T_index bitcount) const;
	//inline void skip_next();
	//inline void skip_next_multi(T_rational::T_index bitcount);
	bitstream& operator>>(bool& bit);
	bitstream& operator<<(const bool bit);
	bitstream& get_multi(T_rational::T_uint_dec& target, T_rational::T_index bitcount);
	bitstream& set_multi(const T_rational::T_uint_dec& source, T_rational::T_index bitcount);
	
	void reset();
};

#endif
