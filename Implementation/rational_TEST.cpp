#include "rational_TEST.h"
#include <iostream>

unsigned char T_rational_TEST::encode_and_decode(T_rational& value_rational,
											T_index& index,
											const T_uint_dec& value_in,
											T_uint_dec& value_out) {
	unsigned char ret = 0;
	T_index i_temp = index;
	ret += value_rational.T_rational::encode_into_bitstream(value_in, index)
			? 0:1;
	ret += value_rational.T_rational::decode_from_bitstream(i_temp, value_out)
			? 0:2;
	return ret;
}

void T_rational_TEST::test_encoding_2nd_Nth() {
	// Instantiate bitset used for encoding
	T_rational value_rational;
	std::cout << "Number of bits to store encoding = "
	<< value_rational.bits_enc.size() << std::endl;
	std::cout << "Bits stored: ";
	for (unsigned int i=0; i<value_rational.bits_enc.size(); ++i) {
		std::cout << (value_rational.bits_enc[i] ? '1':'0');
	}
	std::cout << std::endl;
	// Set value count
	unsigned int valuecount;
	std::cout << "Max number of values to encode >>> ";
	std::cin >> valuecount;
	std::cout << std::endl;
	// Input-encode-decode values individually
	T_uint_dec value_enc;
	T_index index(0);
	std::cout << "Values: " << std::endl;
	for (unsigned int i=1; i<=valuecount; ++i) {
		std::cout << i << ")\t>>> ";
		std::cin >> value_enc;
		if (encode_and_decode(value_rational, index, value_enc, value_enc)) {
			std::cout << "\t -> !failed to encode value!" << std::endl;
			break;
		}
		std::cout << "\t -> " << value_enc << std::endl;
	}
	std::cout << "bits used = " << (int)index << std::endl;
	// Decode values in bulk
	index=0;
	std::cout << "Value list:" << std::endl;
	for (unsigned int i=1; i<=valuecount; ++i) {
		if (!value_rational.decode_from_bitstream(index, value_enc)) {
			std::cout << "\t !failed to decode value!" << std::endl;
		}
		std::cout << "\t" << value_enc << std::endl;
	}
	
	// Output raw bits
	std::cout << "Bits stored: ";
	for (unsigned int i=0; i<value_rational.bits_enc.size(); ++i) {
		std::cout << (value_rational.bits_enc[i] ? '1':'0');
	}

	std::cout << "\nEnd Test" << std::endl;
}



void T_rational_TEST::test_encoding_2nd_Nth_all() {
	T_rational value_rational;
	unsigned char failed_encoding = 0;
	T_uint_dec in;
	T_uint_dec out;
	T_index index;
	std::cout << "Starting Test..." << std::endl;
	for (in=1; !failed_encoding && in!=0; ++in) {
		index=0;
		failed_encoding = encode_and_decode(value_rational, index, in, out);
	}
	if (in == 0) {
		std::cout << "Can encode all values represented in type." << std::endl;
	} else {
		const T_uint_dec end = 2*in;
		in-=2;
		std::cout << "Max value successfully encoded = " << in << std::endl;
		
		for (; in<=end; ++in) {
			index=0;
			value_rational.bits_enc.set();
			switch (encode_and_decode(value_rational, index, in, out)) {
				case 0:
					std::cout << "Successfully encoded " << in
							<< " as " << out << std::endl;
					break;
				case 1:
					std::cout << "Failed to encode " << in << ", "
							<< "successfully decoded " << out << std::endl;
					break;
				case 2:
					std::cout << "Successfully encoded " << in
					<< ", failed to decode" << std::endl;
					break;
				default:
					break;
			}
		}
		std::cout << "Overflowed encoding values failed safely!" << std::endl;
	}
	std::cout << "End Test." << std::endl;
}