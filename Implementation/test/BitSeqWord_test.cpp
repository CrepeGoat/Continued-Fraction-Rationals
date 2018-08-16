#define protected public
#define private public
#include "BitSeqWord.hpp"
#undef private
#undef protected

#include <cstdint>
#include <iostream>
#include <string>
#include <cassert>
#include <exception>
#include <algorithm>
#include <type_traits>
#include <tuple>
//#include "BitTwiddles.h"

static const std::size_t SIZE = 16;
//typedef BitSeqWord<uint8_t>::BitIndex_t BitIndex_t;


// TODO make headers more clear
static void cout_header(
	std::string str_suite,
	std::string str_target, 
	std::string str_testtype
	)
{
	std::cout << "\n===================================\n";
	std::cout << "Test suite:\t" << str_suite << std::endl;
	std::cout << "\ttarget:\t" << str_target << std::endl;
	std::cout << "\ttype:\t" << str_testtype << std::endl;
}

template <typename MBYTE_VALUE, typename INTEGER1, typename INTEGER2>
static inline MBYTE_VALUE effective_bits_of(
	MBYTE_VALUE bits,
	INTEGER1 offset = 0,
	INTEGER2 length = sizeof(MBYTE_VALUE) * CHAR_BIT
	)
{
	bits = shift_right(bits, offset);
	if (length < sizeof(MBYTE_VALUE) * CHAR_BIT) {
		bits = bits & ~(MBYTE_VALUE(-1) << length);
	}
	return bits;
}

template <typename T1, typename T2>
static inline typename max_width<T1, T2>::type min(T1 t1, T2 t2) {
	if (t1 <= t2) {
		return t1;
	} else {
		return t2;
	}
}

template <typename T1, typename T2>
static inline typename max_width<T1, T2>::type max(T1 t1, T2 t2) {
	if (t1 >= t2) {
		return t1;
	} else {
		return t2;
	}
}
/*
template <typename MBYTE, typename INTEGER>
static std::string bits_as_string(
	MBYTE bits,
	INTEGER offset = 0,
	INTEGER length = sizeof(MBYTE) * CHAR_BIT
	)
{
	std::string ret;
	INTEGER i;
	for (INTEGER i = 0; i < offset; ++i, bits >>= 1) {}
	for (; i < offset + length; ++i, bits >>= 1) {
		ret = (bits & 1 ? "1" : "0") + ret;
	}
	return ret;
}
//*/


template <typename... ARGS>
class TestFailureError : public std::logic_error {
	std::tuple<ARGS...> args;
public:
	TestFailureError(std::string msg, ARGS... args)
		:	std::logic_error(msg),
			args(args...)
		{}
};
template <typename... ARGS>
TestFailureError<ARGS...> make_TestFailureError(std::string msg, ARGS... args) {
	return TestFailureError<ARGS...>(msg, args...);
}


/*******************************************************************************
 * TESTS - CONSTRUCTORS
 ******************************************************************************/
template <typename MBYTE, BitAlignment BIT_ALIGN>
void test_BitSeqWord_constructor(
	typename std::remove_reference<MBYTE>::type inner_bits,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t i,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t length
	)
{
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	assert(i + length <= sizeof(MBYTE_VALUE)*CHAR_BIT);

	auto bseqword = BitSeqWord<MBYTE, BIT_ALIGN>(inner_bits, i, i+length);
	
	if (bseqword != effective_bits_of(inner_bits, i, length)) {
		//std::cout << "* Failure!" << std::endl;
		//std::cout << "* \ttrue bits = " << int(bits) << std::endl;
		//std::cout << "* \tinternal bits = "
		//	<< int(bseqword) << std::endl;
		//return false;
		throw make_TestFailureError(
			"bits in BitSeqWord object do not match intended effective bits",
			inner_bits, i, length
			);
	} else if (bseqword.size() != length) {
		throw make_TestFailureError(
			"length of BitSeqWord object bits does not match expected effective length",
			inner_bits, i, length
			);
	} 
}

template <typename MBYTE, BitAlignment BIT_ALIGN>
int autotest_runall_BitSeqWord_constructor() {
	
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef BitSeqWord<MBYTE, BIT_ALIGN> BitSeqWord1;
	
	/*
	cout_header(
		std::is_same<MBYTE, MBYTE_VALUE>::value ? "BitSeqWord class" : "BitSeqWord<lref> class",
		"constructors, get_bits method",
		"pre-defined input range");
	//*/
	int test_counter = 0;
	for (typename BitSeqWord1::BitIndex_t i=0;
			i < BitSeqWord1::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord1::BitIndex_t length=1;
				length < BitSeqWord1::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VALUE effective_bits=0; effective_bits < (1 << length); ++effective_bits) {
				MBYTE_VALUE inner_bits = effective_bits << i;
				
				test_BitSeqWord_constructor<MBYTE, BIT_ALIGN>(inner_bits, i, length);

				++test_counter;
			}
		}
	}
	//std::cout << test_counter << " test cases passed!" << std::endl;
	return test_counter;
}







/*******************************************************************************
 * TESTS - COPY CONSTRUCTION
 ******************************************************************************/
template <typename MBYTE, BitAlignment BIT_ALIGN,
	typename MBYTE2, BitAlignment BIT_ALIGN2>
void test_BitSeqWord_Val_copy_constructor(
	typename std::remove_reference<MBYTE>::type inner_bits,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t i,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t length
	)
{
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef typename std::remove_reference<MBYTE2>::type MBYTE2_VAL; // a little unnecessary but w/e

	assert(i + length <= sizeof(MBYTE_VALUE)*CHAR_BIT);

	auto bseqword = BitSeqWord<MBYTE, BIT_ALIGN>(inner_bits, i, i + length);
	auto bseqword_res = BitSeqWord<MBYTE2, BIT_ALIGN2>(bseqword);

	const auto effective_length = min(length, sizeof(MBYTE2_VAL) * CHAR_BIT);
	const auto effective_bits = effective_bits_of(
			inner_bits, 
			(BIT_ALIGN == BitAlignment::LSB)
				? i : i+length - effective_length,
			effective_length
		);
	if (bseqword_res != effective_bits) {
		//std::cout << "* Failure!" << std::endl;
		//std::cout << "* \ttrue bits = " << int(bits) << std::endl;
		//std::cout << "* \tinternal bits = "
		//	<< int(bseqword_res) << std::endl;
		//return false;
		throw make_TestFailureError(
			"bits in BitSeqWord<VAL> copy do not match expected effective bits",
			inner_bits, i, length
			);
	} else if (bseqword_res.size() != effective_length) {
		throw make_TestFailureError(
			"length of BitSeqWord<VAL> copy bits does not match expected effective length",
			inner_bits, i, length
			);
	}

}



template <typename MBYTE, BitAlignment BIT_ALIGN,
	typename MBYTE2, BitAlignment BIT_ALIGN2>
int autotest_runall_BitSeqWord_copy_constructor() {
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef typename std::remove_reference<MBYTE2>::type MBYTE2_VAL;

	typedef BitSeqWord<MBYTE, BIT_ALIGN> BitSeqWord1;
	typedef BitSeqWord<MBYTE2, BIT_ALIGN2> BitSeqWord2;

	//cout_header(
	//	"BitSeqWord class",
	//	"copy constructors",
	//	"pre-defined input range");

	int test_counter = 0;
	for (typename BitSeqWord1::BitIndex_t i=0;
			i < BitSeqWord1::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord1::BitIndex_t length=1;
				length < BitSeqWord1::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VALUE effective_bits=0; effective_bits < (1 << length);
					++effective_bits) {
				MBYTE_VALUE inner_bits = effective_bits << i;

				test_BitSeqWord_Val_copy_constructor<MBYTE, BIT_ALIGN, MBYTE2, BIT_ALIGN2>(
					inner_bits, i, length);

				++test_counter;
			}
		}
	}
	//std::cout << test_counter << " test cases passed!" << std::endl;
	return test_counter;
}









/*******************************************************************************
 * TESTS - ASSIGNMENT
 ******************************************************************************/
template <typename MBYTE, BitAlignment BIT_ALIGN,
	typename MBYTE2>
void test_BitSeqWord_assignment(
	typename std::remove_reference<MBYTE>::type inner_bits,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t i,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t length,
	typename BitSeqWord<MBYTE2, BIT_ALIGN>::BitIndex_t j,
	typename BitSeqWord<MBYTE2, BIT_ALIGN>::BitIndex_t length2
	)
{
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef typename std::remove_reference<MBYTE2>::type MBYTE2_VAL;

	assert(i + length <= sizeof(MBYTE_VALUE)*CHAR_BIT);
	assert(j + length2 <= sizeof(MBYTE2_VAL)*CHAR_BIT);

	MBYTE2_VAL inner_bits2 = -1;

	auto bseqword = BitSeqWord<MBYTE, BIT_ALIGN>(inner_bits, i, i+length);
	auto bseqword_res = BitSeqWord<MBYTE2, BIT_ALIGN>(inner_bits2, j, j+length2);
	
	// Assignment
	bseqword_res = bseqword;

	MBYTE2_VAL effective_bits;
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t effective_length;
	if (std::is_same<MBYTE2, MBYTE2_VAL>::value) {
		// bit length should be reset entirely
		effective_length = min(length, sizeof(MBYTE2_VAL) * CHAR_BIT);
	} else {
		// bit length either reduced or unchanged
		effective_length = min(length, length2);
	}
	effective_bits = effective_bits_of(
			inner_bits,
			(BIT_ALIGN == BitAlignment::LSB) ? i : i+length - effective_length,
			effective_length
		);

	if (bseqword_res != effective_bits) {
		//std::cout << "* Failure!" << std::endl;
		//std::cout << "* \toriginal bits = " << int(bits) << std::endl;
		//std::cout << "* \teffective original bits = " << int(bits_effective)
		//	<< " [0, " << int(length2) << ")" << std::endl;
		//std::cout << "* \tresulting bits = " << int(bits_res) << std::endl;
		//std::cout << "* \n* \tresult length = " << int(bseqword_res.size()) << std::endl;
		//std::cout << "* \tresult mask() = " << int(bseqword_res.mask()) << std::endl;
		//return false;
		throw make_TestFailureError(
			"bits in BitSeqWord assignment do not match expected effective bits",
			inner_bits, i, length, j, length2
			);
	} else if (bseqword_res.size() != effective_length) {
		throw make_TestFailureError(
			"length of assigned BitSeqWord bits does not match expected effective length",
			inner_bits, i, length, j, length2
			);
	}
}


template <typename MBYTE, BitAlignment BIT_ALIGN,
	typename MBYTE2>
int autotest_runall_BitSeqWord_assignment() {
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef typename std::remove_reference<MBYTE2>::type MBYTE2_VAL;

	typedef BitSeqWord<MBYTE, BIT_ALIGN> BitSeqWord1;
	typedef BitSeqWord<MBYTE2, BIT_ALIGN> BitSeqWord2;

	//cout_header(
	//	std::is_same<MBYTE, MBYTE_VALUE>::value ? "BitSeqWord class" : "BitSeqWord<lref> class",
	//	"assignment methods",
	//	"pre-defined input range"
	//	);

	int test_counter = 0;
	for (typename BitSeqWord1::BitIndex_t i=0;
			i < BitSeqWord1::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord1::BitIndex_t length=1;
				length < BitSeqWord1::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VALUE effective_bits=0; effective_bits < (1 << length);
					++effective_bits) {

				MBYTE_VALUE inner_bits = effective_bits << i;
				
				for (typename BitSeqWord2::BitIndex_t j=0;
						j < BitSeqWord2::BITS_PER_WORD;
						++j) {
					for (typename BitSeqWord2::BitIndex_t length2=1;
							length2 < BitSeqWord2::BITS_PER_WORD - j;
							++length2) {

						test_BitSeqWord_assignment<MBYTE, BIT_ALIGN, MBYTE2>(
							inner_bits, i, length, j, length2);

						++test_counter;
					}
				}
			}
		}
	}
	//std::cout << test_counter << " test cases passed!" << std::endl;
	return test_counter;
}








/*******************************************************************************
 * TESTS - BITWISE OPERATORS
 ******************************************************************************/
template <typename MBYTE, BitAlignment BIT_ALIGN,
	typename MBYTE2>
void test_BitSeqWord_bitwise_xor(
	typename std::remove_reference<MBYTE>::type inner_bits,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t i,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t length,
	typename std::remove_reference<MBYTE2>::type inner_bits2,
	typename BitSeqWord<MBYTE2, BIT_ALIGN>::BitIndex_t j,
	typename BitSeqWord<MBYTE2, BIT_ALIGN>::BitIndex_t length2
	)
{
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef typename std::remove_reference<MBYTE2>::type MBYTE2_VAL;

	auto bseqword = BitSeqWord<MBYTE, BIT_ALIGN>(inner_bits, i, i+length);
	auto bseqword2 = BitSeqWord<MBYTE2, BIT_ALIGN>(inner_bits2, j, j+length2);

	auto bseqword_res = bseqword ^ bseqword2;
	

	auto effective_length = min(length, length2);
	MBYTE_VALUE effective_bits;
	if (BIT_ALIGN == BitAlignment::LSB) {
		effective_bits = effective_bits_of(inner_bits, i, effective_length)
			^ effective_bits_of(inner_bits2, j, effective_length);
	} else {
		effective_bits = effective_bits_of(
				inner_bits,
				i+length - effective_length,
				effective_length
			) ^ effective_bits_of(
				inner_bits2,
				j+length2 - effective_length,
				effective_length
			);
	}

	if (bseqword_res != effective_bits) {
		//std::cout << "* Failure!" << std::endl;
		//std::cout << "* \toriginal bits = " << int(bits) << ", " << int(bits2) << std::endl;
		//std::cout << "* \teffective original bits = "
		//	<< int(bits_effective) << ", " << int(bits2_effective)
		//	<< " [0, " << int(length2) << ")" << std::endl;
		//std::cout << "* \tresulting bits = " << int(bits_res) << std::endl;
		//std::cout << "* \n* \tresult length = " << int(bseqword_res.size()) << std::endl;
		//std::cout << "* \tresult mask() = " << int(bseqword_res.mask()) << std::endl;
		//return false;
		throw make_TestFailureError(
			"bits of BitSeqWord xor do not match expected effective bits",
			inner_bits, i, length, inner_bits2, j, length2
			);
	} else if (bseqword_res.size() != effective_length) {
		throw make_TestFailureError(
			"length of BitSeqWord xor bits does not match expected effective length",
			inner_bits, i, length, inner_bits2, j, length2
			);
	}

}


template <typename MBYTE, BitAlignment BIT_ALIGN,
	typename MBYTE2>
int autotest_runall_BitSeqWord_bitwise_xor() {
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef typename std::remove_reference<MBYTE2>::type MBYTE2_VAL;

	typedef BitSeqWord<MBYTE, BIT_ALIGN> BitSeqWord1;
	typedef BitSeqWord<MBYTE2, BIT_ALIGN> BitSeqWord2;

	//cout_header(
	//	std::is_same<MBYTE, MBYTE_VALUE>::value ? "BitSeqWord class" : "BitSeqWord<lref> class",
	//	"bitwise operations",
	//	"pre-defined input range");

	int test_counter = 0;
	for (typename BitSeqWord1::BitIndex_t i=0;
			i < BitSeqWord1::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord1::BitIndex_t length=1;
				length < BitSeqWord1::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VALUE effective_bits=0; effective_bits < (1 << length);
					++effective_bits) {

				MBYTE_VALUE inner_bits = effective_bits << i;
				
				for (typename BitSeqWord2::BitIndex_t j=0;
						j < BitSeqWord2::BITS_PER_WORD;
						++j) {
					for (typename BitSeqWord2::BitIndex_t length2 = (length > 2 ? length-2 : 1);
							length2 < BitSeqWord2::BITS_PER_WORD - j
								&& length2 < length + 2;
							++length2) {
						for (MBYTE2_VAL effective_bits2=0; effective_bits2 < (1 << length2);
								++effective_bits2) {

							MBYTE2_VAL inner_bits2 = effective_bits2 << j;

							test_BitSeqWord_bitwise_xor<MBYTE, BIT_ALIGN, MBYTE2>(
									inner_bits, i, length,
									inner_bits2, j, length2
								);

							//std::cout << "test " << test_counter << " passed" << std::endl;
							++test_counter;
						}
					}
				}
			}
		}
	}
	//std::cout << test_counter << " test cases passed!" << std::endl;
	return test_counter;
}







/*******************************************************************************
 * TESTS - BITSEARCH (TODO)
 ******************************************************************************/
template <typename MBYTE, BitAlignment BIT_ALIGN>
void test_BitSeqWord_findlsb(
	typename std::remove_reference<MBYTE>::type inner_bits,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t i,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t length,
	bool search_bit
	)
{
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	assert(i + length <= sizeof(MBYTE_VALUE)*CHAR_BIT);

	auto bseqword = BitSeqWord<MBYTE, BIT_ALIGN>(inner_bits, i, i+length);
	
	auto bit_pos_calc = bseqword.find_lsb(search_bit);
	auto bit_pos_true = bit_pos_0h(lsb<MBYTE_VALUE>(
			shift_left<MBYTE_VALUE>(-1, length)
			| (MBYTE_VALUE(search_bit ? 0 : -1) ^ effective_bits_of(inner_bits, i, length))
		));
	
	if (bit_pos_true != bit_pos_calc) {
		//std::cout << "* Failure!" << std::endl;
		//std::cout << "* \ttrue bits = " << int(bits) << std::endl;
		//std::cout << "* \tinternal bits = "
		//	<< int(bseqword) << std::endl;
		//return false;
		throw make_TestFailureError(
			"position of first bit in BitSeqWord object does not match intended position",
			inner_bits, i, length
			);
	} 
}

template <typename MBYTE, BitAlignment BIT_ALIGN>
void test_BitSeqWord_findmsb(
	typename std::remove_reference<MBYTE>::type inner_bits,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t i,
	typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t length,
	bool search_bit
	)
{
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	assert(i + length <= sizeof(MBYTE_VALUE)*CHAR_BIT);

	auto bseqword = BitSeqWord<MBYTE, BIT_ALIGN>(inner_bits, i, i+length);
	
	auto bit_pos_calc = bseqword.find_msb(search_bit);
	auto bit_pos_true = bit_pos_0l(msb<MBYTE_VALUE>(
			~shift_left<MBYTE_VALUE>(-1, length)
			& (MBYTE_VALUE(search_bit ? 0 : -1) ^ effective_bits_of(inner_bits, i, length))
		));
	
	if (bit_pos_true != bit_pos_calc) {
		//std::cout << "* Failure!" << std::endl;
		//std::cout << "* \ttrue bits = " << int(bits) << std::endl;
		//std::cout << "* \tinternal bits = "
		//	<< int(bseqword) << std::endl;
		//return false;
		throw make_TestFailureError(
			"position of first bit in BitSeqWord object does not match intended position",
			inner_bits, i, length
			);
	} 
}

template <typename MBYTE, BitAlignment BIT_ALIGN>
int autotest_runall_BitSeqWord_findlmsb() {
	
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef BitSeqWord<MBYTE, BIT_ALIGN> BitSeqWord1;
	
	/*
	cout_header(
		std::is_same<MBYTE, MBYTE_VALUE>::value ? "BitSeqWord class" : "BitSeqWord<lref> class",
		"constructors, get_bits method",
		"pre-defined input range");
	//*/
	int test_counter = 0;
	for (typename BitSeqWord1::BitIndex_t i=0;
			i < BitSeqWord1::BITS_PER_WORD;
			++i) {
		for (typename BitSeqWord1::BitIndex_t length=1;
				length < BitSeqWord1::BITS_PER_WORD - i;
				++length) {
			for (MBYTE_VALUE effective_bits=0; effective_bits < (1 << length); ++effective_bits) {
				MBYTE_VALUE inner_bits = effective_bits << i;
				
				test_BitSeqWord_findlsb<MBYTE, BIT_ALIGN>(inner_bits, i, length, false);
				test_BitSeqWord_findlsb<MBYTE, BIT_ALIGN>(inner_bits, i, length, true);
				test_BitSeqWord_findmsb<MBYTE, BIT_ALIGN>(inner_bits, i, length, false);
				test_BitSeqWord_findmsb<MBYTE, BIT_ALIGN>(inner_bits, i, length, true);

				++test_counter;
			}
		}
	}
	//std::cout << test_counter << " test cases passed!" << std::endl;
	return test_counter;
}


void run_tests() {

	autotest_runall_BitSeqWord_constructor<uint8_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_constructor<uint8_t&, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_constructor<uint16_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_constructor<uint16_t&, BitAlignment::LSB>();

	autotest_runall_BitSeqWord_constructor<uint8_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_constructor<uint8_t&, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_constructor<uint16_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_constructor<uint16_t&, BitAlignment::MSB>();
	

	
	autotest_runall_BitSeqWord_copy_constructor<uint8_t, BitAlignment::LSB, uint16_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint16_t, BitAlignment::LSB, uint8_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint8_t&, BitAlignment::LSB, uint16_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint16_t&, BitAlignment::LSB, uint8_t, BitAlignment::LSB>();

	autotest_runall_BitSeqWord_copy_constructor<uint8_t, BitAlignment::LSB, uint8_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint8_t&, BitAlignment::LSB, uint8_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint16_t, BitAlignment::LSB, uint16_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint16_t&, BitAlignment::LSB, uint16_t, BitAlignment::MSB>();

	autotest_runall_BitSeqWord_copy_constructor<uint8_t, BitAlignment::MSB, uint8_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint8_t&, BitAlignment::MSB, uint8_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint16_t, BitAlignment::MSB, uint16_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint16_t&, BitAlignment::MSB, uint16_t, BitAlignment::LSB>();

	autotest_runall_BitSeqWord_copy_constructor<uint8_t, BitAlignment::MSB, uint16_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint16_t, BitAlignment::MSB, uint8_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint8_t&, BitAlignment::MSB, uint16_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_copy_constructor<uint16_t&, BitAlignment::MSB, uint8_t, BitAlignment::MSB>();

	

	autotest_runall_BitSeqWord_assignment<uint8_t, BitAlignment::LSB, uint8_t>();
	autotest_runall_BitSeqWord_assignment<uint8_t, BitAlignment::LSB, uint8_t&>();
	autotest_runall_BitSeqWord_assignment<uint16_t, BitAlignment::LSB, uint8_t>();
	autotest_runall_BitSeqWord_assignment<uint16_t, BitAlignment::LSB, uint8_t&>();
	autotest_runall_BitSeqWord_assignment<uint8_t, BitAlignment::LSB, uint16_t>();
	autotest_runall_BitSeqWord_assignment<uint8_t, BitAlignment::LSB, uint16_t&>();

	autotest_runall_BitSeqWord_assignment<uint8_t, BitAlignment::MSB, uint8_t>();
	autotest_runall_BitSeqWord_assignment<uint8_t, BitAlignment::MSB, uint8_t&>();
	autotest_runall_BitSeqWord_assignment<uint16_t, BitAlignment::MSB, uint8_t>();
	autotest_runall_BitSeqWord_assignment<uint16_t, BitAlignment::MSB, uint8_t&>();
	autotest_runall_BitSeqWord_assignment<uint8_t, BitAlignment::MSB, uint16_t>();
	autotest_runall_BitSeqWord_assignment<uint8_t, BitAlignment::MSB, uint16_t&>();



	autotest_runall_BitSeqWord_bitwise_xor<uint8_t, BitAlignment::LSB, uint8_t&>();
	autotest_runall_BitSeqWord_bitwise_xor<uint16_t, BitAlignment::LSB, uint8_t>();
	autotest_runall_BitSeqWord_bitwise_xor<uint16_t, BitAlignment::LSB, uint8_t&>();
	autotest_runall_BitSeqWord_bitwise_xor<uint8_t, BitAlignment::LSB, uint16_t>();
	autotest_runall_BitSeqWord_bitwise_xor<uint8_t, BitAlignment::LSB, uint16_t&>();

	autotest_runall_BitSeqWord_bitwise_xor<uint8_t, BitAlignment::MSB, uint8_t&>();
	autotest_runall_BitSeqWord_bitwise_xor<uint16_t, BitAlignment::MSB, uint8_t>();
	autotest_runall_BitSeqWord_bitwise_xor<uint16_t, BitAlignment::MSB, uint8_t&>();
	autotest_runall_BitSeqWord_bitwise_xor<uint8_t, BitAlignment::MSB, uint16_t>();
	autotest_runall_BitSeqWord_bitwise_xor<uint8_t, BitAlignment::MSB, uint16_t&>();



	autotest_runall_BitSeqWord_findlmsb<uint8_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_findlmsb<uint8_t&, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_findlmsb<uint16_t, BitAlignment::LSB>();
	autotest_runall_BitSeqWord_findlmsb<uint16_t&, BitAlignment::LSB>();

	autotest_runall_BitSeqWord_findlmsb<uint8_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_findlmsb<uint8_t&, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_findlmsb<uint16_t, BitAlignment::MSB>();
	autotest_runall_BitSeqWord_findlmsb<uint16_t&, BitAlignment::MSB>();

	std::cout << "Tests passed!" << std::endl;
}

int main() {
	run_tests();
	return 0;
}
