#include <bitset>

#ifndef RATIONAL_H
#define RATIONAL_H

/* TODO List:
 *		- make/use bitstream class for template-adjustable implementation
 *		- consider sizeless rational number class
 *
 */

class T_rational_TEST;

class T_rational {
private:
	friend T_rational_TEST;
	// Typedefs/Const Values
	typedef unsigned long int T_uint_dec;
	typedef long int T_int_dec;
	typedef unsigned char T_index;
	
	static const bool _0 = false;
	static const bool _1 = true;
	
	// Internal Variables
	std::bitset<8*(4)> bits_enc;
	
	// Support Functions
	////////// TODO test //////////////
	static bool get_nth_digit_v1(const T_index digit, const T_uint_dec& value);
	static bool get_nth_digit_v2(const T_index digit, const T_uint_dec& value);
	static bool get_nth_digit_v3(const T_index digit, const T_uint_dec& value);
	static inline bool get_nth_digit(const T_index digit, const T_uint_dec& value);
	static inline void set_nth_digit(const T_index digit, T_uint_dec& value, bool bit);
	static T_index log2(const T_uint_dec& x);
	
	/*
	static inline T_rational::T_index no_of_bits_in_encoding_of(const T_uint_dec& value);
	static inline T_rational::T_uint_dec max_value_for_bitcount_of(T_index bitcount);
	inline bool can_encode_value_in_bitset_v1(const T_uint_dec& value_dec, T_index& i_enc) const;
	inline bool can_encode_value_in_bitset_v2(const T_uint_dec& value_dec, T_index& i_enc) const;
	inline bool can_encode_value_in_bitset(const T_uint_dec& value_dec, T_index& i_enc) const;
	*/
	
	
	// Utility Functions
	bool encode_into_bitstream(const T_uint_dec& value_dec,
							   T_index& i_enc);
	bool decode_from_bitstream(T_index& i_enc,
							   T_uint_dec& value_dec) const;
	bool encode_1st_into_bitstream(const T_int_dec& value_dec,
								   T_index& i_enc);
	bool decode_1st_from_bitstream(T_index& i_enc,
								   T_int_dec& value_dec) const;
	
public:
	// Constructors
	T_rational();
	//template <typename P_INT>
	//t_float_contfrac(P_INT p_begin, P_INT p_end);
	
	// Float Casting
	//operator float();
};

#endif
