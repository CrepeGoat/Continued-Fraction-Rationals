#include <bitset>

#ifndef RATIONAL_TEST_H
#define RATIONAL_TEST_H

#include "rational.h"

//class T_rational;

namespace T_rational_TEST {
	typedef T_rational::T_uint_dec T_uint_dec;
	typedef T_rational::T_index T_index;
		
	// Test T_rational::bitstream member functions
	void test_bitstream_in_out();
	void test_bitstream_in_out_multi();
	
	// Test T_rational member functions
	unsigned char encode_and_decode(T_rational& value_rational,
									T_rational::bitstream& bstream,
									const T_uint_dec& value_in,
									T_uint_dec& value_out);
	void test_encoding_2nd_Nth_v1();
	void test_encoding_2nd_Nth_v2();
	void test_decoding_2nd_Nth();
	
};

#endif
