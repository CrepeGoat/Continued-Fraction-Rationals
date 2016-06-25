#include <bitset>

#ifndef RATIONAL_TEST_H
#define RATIONAL_TEST_H

#include "rational.h"

//class T_rational;

class T_rational_TEST {
public:
	typedef T_rational::T_uint_dec T_uint_dec;
	typedef T_rational::T_index T_index;
	
	static unsigned char encode_and_decode(T_rational& value_rational,
								  T_index& index,
								  const T_uint_dec& value_in,
								  T_uint_dec& value_out);
	static void test_encoding_2nd_Nth();
	static void test_encoding_2nd_Nth_all();
};

#endif
