#define protected public
#define private public
#include "BitSeqWord.h"
#undef private
#undef protected


#include <iostream>
#include <string>
#include <algorithm>
#include <type_traits>
#include <initializer_list>
//#include "BitTwiddles.h"

static const std::size_t SIZE = 16;
typedef unsigned char byte;
typedef unsigned short mbyte;
typedef BitSeqWord<byte>::BitIndex_t BitIndex_t;


// TODO make headers more clear
static void cout_header(
	std::string str_suite,
	std::string str_target, 
	std::string str_testtype)
{
	std::cout << "\n===================================\n";
	std::cout << "Test suite:\t" << str_suite << std::endl;
	std::cout << "\ttarget:\t" << str_target << std::endl;
	std::cout << "\ttype:\t" << str_testtype << std::endl;
}

template <typename MBYTE, typename INTEGER>
static std::string bits_as_string(MBYTE bits, INTEGER offset, INTEGER length) {
	std::string ret;
	INTEGER i;
	for (INTEGER i = 0; i < offset; ++i, bits >>= 1) {}
	for (; i < offset + length; ++i, bits >>= 1) {
		ret = (bits & 1 ? "1" : "0") + ret;
	}
	return ret;
}
/*******************************************************************************
 * TESTS - CONSTRUCTORS
 ******************************************************************************/
template <typename MBYTE>
bool test_BitSeqWord_constructors() {
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VAL;

	cout_header(
		std::is_same<MBYTE, MBYTE_VAL>::value ? "BitSeqWord class" : "BitSeqWord<lref> class",
		"constructors, get_bits method",
		"pre-defined input range");

	int test_counter = 0;
	for (typename BitSeqWord<MBYTE>::BitIndex_t i=0;
			i < BitSeqWord<MBYTE>::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord<MBYTE>::BitIndex_t length=1;
				length < BitSeqWord<MBYTE>::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VAL bits=0; bits < (1 << length);
					++bits) {
				MBYTE_VAL val = bits << i;
				
				auto bseqword = BitSeqWord<MBYTE>(val, i, i+length);
				if (bits != bseqword.get_bits()) {
					std::cout << "* Failure!" << std::endl;
					std::cout << "* \ttrue bits = " << int(bits) << std::endl;
					std::cout << "* \tinternal bits = "
						<< int(bseqword.get_bits()) << std::endl;
					return false;
				}
				++test_counter;
			}
		}
	}
	std::cout << test_counter << " test cases passed!" << std::endl;
	return true;
}

/*******************************************************************************
 * TESTS - COPY CONSTRUCTION
 ******************************************************************************/
template <typename MBYTE, typename NBYTE>
bool test_BitSeqWord_copy_constructors() {
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VAL;
	typedef typename std::remove_reference<NBYTE>::type NBYTE_VAL;

	cout_header(
		"BitSeqWord class",
		"copy constructors",
		"pre-defined input range");

	int test_counter = 0;
	for (typename BitSeqWord<MBYTE>::BitIndex_t i=0;
			i < BitSeqWord<MBYTE>::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord<MBYTE>::BitIndex_t length=1;
				length < BitSeqWord<MBYTE>::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VAL bits=0; bits < (1 << length);
					++bits) {
				MBYTE_VAL val = bits << i;
				
				auto bseqword = BitSeqWord<MBYTE>(val, i, i+length);
				auto bseqword2 = BitSeqWord<NBYTE>(bseqword);

				const MBYTE_VAL bits_effective = bits & mask<MBYTE_VAL>(
						0, BitSeqWordBase<typename min_width<MBYTE, NBYTE>::value_type>::BITS_PER_WORD);

				if (bits_effective != bseqword2.get_bits()) {
					std::cout << "* Failure!" << std::endl;
					std::cout << "* \ttrue bits = " << int(bits) << std::endl;
					std::cout << "* \tinternal bits = "
						<< int(bseqword2.get_bits()) << std::endl;
					return false;
				}
				++test_counter;
			}
		}
	}
	std::cout << test_counter << " test cases passed!" << std::endl;
	return true;
}


/*******************************************************************************
 * TESTS - ASSIGNMENT
 ******************************************************************************/
template <typename MBYTE, typename NBYTE>
bool test_BitSeqWord_assignment() {
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VAL;
	typedef typename std::remove_reference<NBYTE>::type NBYTE_VAL;

	cout_header(
		std::is_same<MBYTE, MBYTE_VAL>::value ? "BitSeqWord class" : "BitSeqWord<lref> class",
		"assignment methods",
		"pre-defined input range");

	int test_counter = 0;
	for (typename BitSeqWord<MBYTE>::BitIndex_t i=0;
			i < BitSeqWord<MBYTE>::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord<MBYTE>::BitIndex_t length=1;
				length < BitSeqWord<MBYTE>::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VAL bits=0; bits < (1 << length);
					++bits) {

				MBYTE_VAL val = bits << i;
				auto bseqword = BitSeqWord<MBYTE>(val, i, i+length);
				
				for (typename BitSeqWord<NBYTE>::BitIndex_t j=0;
						j < BitSeqWord<NBYTE>::BITS_PER_WORD;
						++j) {
					for (typename BitSeqWord<NBYTE>::BitIndex_t length2=1;
							length2 < BitSeqWord<NBYTE>::BITS_PER_WORD - j;
							++length2) {

						NBYTE_VAL val2 = 0;
						auto bseqword_res = BitSeqWord<NBYTE>(val2, j, j+length2);
						// Assignment
						bseqword_res = bseqword;

						const MBYTE_VAL bits_effective = bits & mask<MBYTE_VAL>(
								0, MBYTE_VAL(std::is_same<NBYTE, NBYTE_VAL>::value
									? BitSeqWordBase<typename min_width<MBYTE, NBYTE>::value_type>::BITS_PER_WORD
									: std::min(length, length2)
								)
							);
						NBYTE_VAL bits_res = bseqword_res.get_bits();

						if (bits_effective != bits_res) {
							std::cout << "* Failure!" << std::endl;
							std::cout << "* \toriginal bits = " << int(bits) << std::endl;
							std::cout << "* \teffective original bits = " << int(bits_effective)
								<< " [0, " << int(length2) << ")" << std::endl;
							std::cout << "* \tresulting bits = " << int(bits_res) << std::endl;
							std::cout << "* \n* \tresult length = " << int(bseqword_res.size()) << std::endl;
							std::cout << "* \tresult mask() = " << int(bseqword_res.mask()) << std::endl;
							return false;
						}
						//std::cout << "test " << test_counter << " passed" << std::endl;
						++test_counter;
					}
				}
			}
		}
	}
	std::cout << test_counter << " test cases passed!" << std::endl;
	return true;
}

/*******************************************************************************
 * TESTS - BITWISE OPERATORS
 ******************************************************************************/
template <typename MBYTE, typename NBYTE>
bool test_BitSeqWord_bitwise_operations() {
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VAL;
	typedef typename std::remove_reference<NBYTE>::type NBYTE_VAL;

	cout_header(
		std::is_same<MBYTE, MBYTE_VAL>::value ? "BitSeqWord class" : "BitSeqWord<lref> class",
		"bitwise operations",
		"pre-defined input range");

	int test_counter = 0;
	for (typename BitSeqWord<MBYTE>::BitIndex_t i=0;
			i < BitSeqWord<MBYTE>::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord<MBYTE>::BitIndex_t length=1;
				length < BitSeqWord<MBYTE>::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VAL bits=0; bits < (1 << length);
					++bits) {

				MBYTE_VAL val = bits << i;
				auto bseqword = BitSeqWord<MBYTE>(val, i, i+length);
				
				for (typename BitSeqWord<NBYTE>::BitIndex_t j=0;
						j < BitSeqWord<NBYTE>::BITS_PER_WORD;
						++j) {
					for (typename BitSeqWord<NBYTE>::BitIndex_t length2 = (length > 2 ? length-2 : 1);
							length2 < BitSeqWord<NBYTE>::BITS_PER_WORD - j
								&& length2 < length + 2;
							++length2) {
						for (NBYTE_VAL bits2=0; bits2 < (1 << length2);
								++bits2) {

							NBYTE_VAL val2 = bits2 << j;
							auto bseqword2 = BitSeqWord<NBYTE>(val2, j, j+length2);
							// 

							MBYTE_VAL bits_effective = bits & mask<MBYTE_VAL>(
									0, std::min(length, length2)
								);
							NBYTE_VAL bits2_effective = bits2 & mask<NBYTE_VAL>(
									0, std::min(length, length2)
								);

							auto bseqword_res = bseqword ^ bseqword2;
							auto bits_res = bseqword_res.get_bits();

							if (bits_res != (bits_effective ^ bits2_effective)) {
								std::cout << "* Failure!" << std::endl;
								std::cout << "* \toriginal bits = " << int(bits) << ", " << int(bits2) << std::endl;
								std::cout << "* \teffective original bits = "
									<< int(bits_effective) << ", " << int(bits2_effective)
									<< " [0, " << int(length2) << ")" << std::endl;
								std::cout << "* \tresulting bits = " << int(bits_res) << std::endl;
								std::cout << "* \n* \tresult length = " << int(bseqword_res.size()) << std::endl;
								std::cout << "* \tresult mask() = " << int(bseqword_res.mask()) << std::endl;
								return false;
							}
							//std::cout << "test " << test_counter << " passed" << std::endl;
							++test_counter;
						}
					}
				}
			}
		}
	}
	std::cout << test_counter << " test cases passed!" << std::endl;
	return true;
}

void run_tests() {//std::initializer_list<bool (**)()> tests) {

	//for (auto iter = tests.begin(); iter < tests.end(); ++iter) {
	//	if (!(**iter)()) {
	//		return;
	//	}
	//}

	bool status = true;

	//*
	status &= test_BitSeqWord_constructors<byte>();
	status &= test_BitSeqWord_constructors<byte&>();
	status &= test_BitSeqWord_constructors<mbyte>();
	status &= test_BitSeqWord_constructors<mbyte&>();

	//*
	status &= test_BitSeqWord_copy_constructors<byte, mbyte>();
	status &= test_BitSeqWord_copy_constructors<mbyte, byte>();
	status &= test_BitSeqWord_copy_constructors<byte&, mbyte>();
	status &= test_BitSeqWord_copy_constructors<mbyte&, byte>();

	//*
	status &= test_BitSeqWord_assignment<byte, byte&>();
	status &= test_BitSeqWord_assignment<mbyte, byte>();
	status &= test_BitSeqWord_assignment<mbyte, byte&>();
	status &= test_BitSeqWord_assignment<byte, mbyte>();
	status &= test_BitSeqWord_assignment<byte, mbyte&>();
	//*/
	
	//*
	status &= test_BitSeqWord_bitwise_operations<byte, byte&>();
	status &= test_BitSeqWord_bitwise_operations<mbyte, byte>();
	status &= test_BitSeqWord_bitwise_operations<mbyte, byte&>();
	status &= test_BitSeqWord_bitwise_operations<byte, mbyte>();
	status &= test_BitSeqWord_bitwise_operations<byte, mbyte&>();
	//*/

	if (status) {
		std::cout << "\n***********************"
				  << "\n***All tests passed!***"
				  << "\n***********************"
				  << std::endl;
	}
}

int main() {
	/*
	run_tests({
		//*
		&test_BitSeqWord_constructors<byte>,
		&test_BitSeqWord_constructors<byte&>,
		&test_BitSeqWord_constructors<mbyte>,
		&test_BitSeqWord_constructors<mbyte&>,

		&test_BitSeqWord_copy_constructors<byte, mbyte>,
		&test_BitSeqWord_copy_constructors<mbyte, byte>,
		&test_BitSeqWord_copy_constructors<byte&, mbyte>,
		&test_BitSeqWord_copy_constructors<mbyte&, byte>,

		&test_BitSeqWord_assignment<mbyte, byte>,
		&test_BitSeqWord_assignment<byte, mbyte>,
		&test_BitSeqWord_assignment<mbyte, byte&>,
		&test_BitSeqWord_assignment<byte, mbyte&>,
		&test_BitSeqWord_assignment<byte, byte&>
	});
	//*/

	run_tests();
	return 0;
}
