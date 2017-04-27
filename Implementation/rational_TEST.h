#ifndef RATIONAL_TEST_H
#define RATIONAL_TEST_H

#include "rational.h"

//class T_rational;

namespace T_rational_TEST {
	typedef T_rational<4>::T_uint_dec T_uint_dec;
	typedef T_rational<4>::T_index T_index;
		
	// Test T_rational::bitstream member functions
	void test_bitstream_in_out();
	void test_bitstream_in_out_multi();
	
	// Test T_rational member functions
	template <unsigned char BYTE_COUNT>
	unsigned char encode_and_decode(T_rational<BYTE_COUNT>& value_rational,
									typename T_rational<BYTE_COUNT>::bitstream& bstream,
									const T_uint_dec& value_in,
									T_uint_dec& value_out) {
		unsigned char ret = 0;
		typename T_rational<BYTE_COUNT>::bitstream bstream_copy = bstream;
		ret += value_rational.T_rational<BYTE_COUNT>::encode_into_bitstream(value_in, bstream) ? 0:1;
		ret += value_rational.T_rational<BYTE_COUNT>::decode_from_bitstream(bstream_copy, value_out) ? 0:2;
		return ret;
		
	}
	void test_encoding_2nd_Nth_v1();
	void test_encoding_2nd_Nth_v2();
	void test_decoding_2nd_Nth();
	
};

#endif
