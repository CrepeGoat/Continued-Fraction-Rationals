#include "rational_TEST.h"
#include <iostream>

/* Bitstream tests
 *
 *
 *
 *
 *
 *
 *
 *
 */

void T_rational_TEST::test_bitstream_in_out() {
	// Variables
	T_rational value_rational;
	T_rational::bitstream bstream(value_rational);
	char c;
	
	// Input loop
	do {
		std::cout << "Enter next bool (q to quit) >>> ";
		std::cin >> c;
		switch (c) {
			case '1':
			case 't':
			case 'T':
				bstream << true;
				break;
			case '0':
			case 'f':
			case 'F':
				bstream << false;
				break;
			case 'q':
			case 'Q':
				std::cout << "End input" << std::endl;
				break;
			default:
				std::cout << "Invalid input" << std::endl;
		}
	} while (bstream.has_next() && c!='Q' && c!='q');
	
	// Verify output
	std::cout << "Bits stored: " << value_rational.bits_enc << std::endl;
	
	// Output Loop
	std::cout << "Bits via single output stream: ";
	bstream.reset();
	bool bool_tmp;
	while (bstream.has_next()) {
		bstream >> bool_tmp;
		std::cout << (bool_tmp ? 1:0);
	}
	std::cout << std::endl;
	
	// End Test
	std::cout << "End Test." << std::endl;
}
void T_rational_TEST::test_bitstream_in_out_multi() {
	// Variables
	T_rational value_rational;
	T_rational::bitstream bstream(value_rational);
	char c;
	T_uint_dec bits_tmp=0;
	unsigned short bitcount;
	
	// Input loop
	while (bstream.has_next()) {
		bitcount = 0;
		do {
			std::cout << "Enter next bool (q to quit) >>> ";
			std::cin >> c;
			switch (c) {
				case '1':
				case 't':
				case 'T':
					bits_tmp <<= 1;
					bits_tmp |= 1;
					++bitcount;
					break;
				case '0':
				case 'f':
				case 'F':
					bits_tmp <<= 1;
					++bitcount;
					break;
				case 'q':
				case 'Q':
					std::cout << "End input" << std::endl;
					break;
				default:
					std::cout << "Invalid input" << std::endl;
			}
		} while (c!='Q' && c!='q');
		if (!bstream.has_next_multi(bitcount) || bitcount==0)
			break;
		// Add stored bits to bitarray
		bstream.set_multi(bits_tmp, bitcount);
		// Verify output
		std::cout << "Bits stored: " << value_rational.bits_enc << std::endl;
	}
	// Output Loop
	std::cout << "Bits via multi-output stream" << std::endl;
	bstream.reset();
	while (bstream.has_next()) {
		bits_tmp = 0;
		std::cout << "Enter no. of bits to retrieve (0 to quit) >>> ";
		std::cin >> bitcount;
		if (!(std::cin))
			std::cout << "Invalid input" << std::endl;
		else if (bitcount == 0)
			break;
		bstream.get_multi(bits_tmp, bitcount);
		std::cout << "Retrieved bits: " << std::hex << bits_tmp << std::endl;
	}
	std::cout << std::endl;
	
	// End Test
	std::cout << "End Test." << std::endl;
}


/* Number encoding tests
 *
 *
 *
 *
 *
 *
 *
 *
 */
unsigned char T_rational_TEST::encode_and_decode(T_rational& value_rational,
												 T_rational::bitstream& bstream,
												 const T_uint_dec& value_in,
												 T_uint_dec& value_out) {
	unsigned char ret = 0;
	T_rational::bitstream bstream_copy = bstream;
	ret += value_rational.T_rational::encode_into_bitstream(value_in, bstream) ? 0:1;
	ret += value_rational.T_rational::decode_from_bitstream(bstream_copy, value_out) ? 0:2;
	return ret;
}

void T_rational_TEST::test_encoding_2nd_Nth_v1() {
	// Instantiate bitset used for encoding
	T_rational value_rational;
	std::cout << "Number of bits to store encoding = "
	<< value_rational.bits_enc.size() << std::endl;
	std::cout << "Bits stored: " << value_rational.bits_enc << std::endl;
	
	// Set value count
	unsigned int valuecount;
	std::cout << "Max number of values to encode >>> ";
	std::cin >> valuecount;
	std::cout << std::endl;
	
	// Input-encode-decode values individually
	T_uint_dec value_enc;
	T_rational::bitstream bstream(value_rational);
	std::cout << "Values: " << std::endl;
	for (unsigned int i=1; i<=valuecount; ++i) {
		std::cout << i << ")\t>>> ";
		std::cin >> value_enc;
		if (encode_and_decode(value_rational, bstream, value_enc, value_enc)) {
			std::cout << "\t -> !failed to encode value!" << std::endl;
			break;
		}
		std::cout << "\t -> " << value_enc << std::endl;
	}
	std::cout << "bits used = " << bstream.bits_enc.size()-bstream.index << std::endl;
	
	// Decode values sequentially
	bstream.reset();
	std::cout << "Value list:" << std::endl;
	for (unsigned int i=1; i<=valuecount; ++i) {
		if (!value_rational.decode_from_bitstream(bstream, value_enc)) {
			std::cout << "\t !failed to decode value!" << std::endl;
		}
		std::cout << "\t" << value_enc << std::endl;
	}
	
	// Output raw bits
	std::cout << "Bits stored: " << value_rational.bits_enc << std::endl;

	std::cout << "\nEnd Test" << std::endl;
}



void T_rational_TEST::test_encoding_2nd_Nth_v2() {
	// Declare variables
	T_rational value_rational;
	unsigned char failed_encoding = 0;
	T_uint_dec in;
	T_uint_dec out;
	T_rational::bitstream bstream(value_rational);
	// Start test
	std::cout << "Starting Test..." << std::endl;
	for (in=1; !failed_encoding && in!=0; ++in) {
		bstream.reset();
		failed_encoding = encode_and_decode(value_rational, bstream, in, out);
	}
	if (in == 0) {
		std::cout << "Can encode all values represented in type." << std::endl;
	} else {
		const T_uint_dec end = 2*in;
		in-=2;
		std::cout << "Max value successfully encoded = " << in << std::endl;
		
		for (; in<=end; ++in) {
			bstream.reset();
			value_rational.bits_enc.set();
			switch (encode_and_decode(value_rational, bstream, in, out)) {
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

void T_rational_TEST::test_decoding_2nd_Nth() {
	T_rational value_rational;
	T_uint_dec in;
	T_uint_dec out;
	T_rational::bitstream bstream(value_rational);
	
	std::cout << "Value to partially encode >>> ";
	std::cin >> in;
	std::cout << std::endl;
	
	if (!value_rational.encode_into_bitstream(in, bstream)) {
		std::cout << "Failed to encode " << in << ", aborting test" << std::endl;
		return;
	}
	
	bstream.reset();
	for (T_index index = value_rational.bits_enc.size()-1; index!=(T_index)-1; --index) {
		value_rational.bits_enc >>= 1;
		bstream.index = index;
		try {
			if (value_rational.decode_from_bitstream(bstream, out)) {
				std::cout << "Successfully decoded " << out << std::endl;
			} else {
				std::cout << "Failed to decode. Safe exit." << std::endl;
			}
		} catch (const std::exception e) {
			std::cout << "Failed to decode. Exception thrown on exit." << std::endl;
		}
	}
	std::cout << "End Test." << std::endl;
}
