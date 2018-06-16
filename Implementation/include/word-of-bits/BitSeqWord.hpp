#ifndef BITSEQWORD_HPP
#define BITSEQWORD_HPP

#include "TypeUtility.hpp"

#include <climits>		// CHAR_BIT
#include <cstdint>		// intN_t
#include <type_traits>	// std::conditional


/*

This is a wrapper class designed to automatically shift & mask when writing over
specified bit regions from a source unaligned with the destination.

The general mindset for this design is as follows:
	- an object of this class is just a sequence of bits in a small box

	- no two sequences are distinguished by word-wise alignment
		- e.g., 11[00101]1 and [00101]000 are considered equivalent
	
	- operations are performed bitwise, with each respective range aligned along
		the least-significant bit
		- e.g., 001[11000] & [00111]000 -> [11111]
		- TODO - templatize to allow most-significant alignment
	
	- if two boxes have different numbers of bits, **the excess bits are discarded**
		-> the length of the resulting BitSeqWord object at the end of any operations
			is **precisely** the number of bits that were successfully involved in
			**all** of the operations
		- if too many bits were discarded, the user will need to regenerate the results
			from the beginning, shifting the input words to obtain the desired bits

	- there are two partial specializations: MBYTE, and MBYTE&
		
		MBYTE& - wraps an existing MBYTE object to (eventually) store the results
				of bitwise operations
			- only the bits explicity within its range are to be affected by any operations
			* -> local shifts are forbidden
			* -> any assignments must be bit-masked onto active range
		
		MBYTE - owns its own local word object and may change it as required
			* -> local shifts are permitted
			* -> assignments may overwrite the whole word w/o consequence
		
		- combining the two creates an abstraction for:
			- bitwise operations and assignments of (sub-)word bit chunks
			- omits concern for alignment of sequences
			- omits concern for equal sequence lengths (via auto-discard & size() method)
			- performs a minimum number of shift operations per operation

Other thoughts on internals:
	- internal shifting should be as lazy as possible
		-> track both range's lower and upper bounds
		- (as opposed to keeping range aligned along word edge)

TODO
	- test/debug
	- implement bitwise operators
		? also implement shift_alignment (much like >>)
	- add template parameter, bool ALIGN_LSB
		- determines which bits are prioritized when discarding excess

 */


enum class BitAlignment : int8_t {LSB, MSB};
constexpr BitAlignment invert(BitAlignment align);


template <typename MBYTE, BitAlignment BIT_ALIGN>
class BitSeqWord;


//////////////////////////////////////////
// BASE CLASS DEFINITION
//////////////////////////////////////////

template <typename MBYTE>//, BitAlignment BIT_ALIGN>
class BitSeqWordBase {
public:
	using BitIndex_t = typename std::conditional<
			sizeof(MBYTE)*CHAR_BIT <= INT8_MAX,
			int8_t, int16_t
		>::type;
	
	static constexpr BitIndex_t BITS_PER_WORD = sizeof(MBYTE)*CHAR_BIT;

protected:
	BitIndex_t bit_margin_lsb, bit_margin_msb;

	MBYTE mask() const;

	BitIndex_t bit_non_lsbmargin() const;
	BitIndex_t bit_non_msbmargin() const;	

public:
	BitSeqWordBase(BitIndex_t, BitIndex_t);

	BitIndex_t size() const;
};

//////////////////////////////////////////
// GLOBAL OVERRIDE DEFINITIONS
//////////////////////////////////////////

template <typename MBYTE, typename MBYTE2, BitAlignment BIT_ALIGN, typename OPERATOR>
static inline BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN> get_bitwise_operation(
	const BitSeqWord<MBYTE, BIT_ALIGN>&, const BitSeqWord<MBYTE2, BIT_ALIGN>&, OPERATOR);	

template <typename MBYTE, typename MBYTE2, BitAlignment BIT_ALIGN>
BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN> operator&(
		const BitSeqWord<MBYTE, BIT_ALIGN>& word_m, const BitSeqWord<MBYTE2, BIT_ALIGN>& word_n);
template <typename MBYTE, typename MBYTE2, BitAlignment BIT_ALIGN>
BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN> operator|(
		const BitSeqWord<MBYTE, BIT_ALIGN>& word_m, const BitSeqWord<MBYTE2, BIT_ALIGN>& word_n);
template <typename MBYTE, typename MBYTE2, BitAlignment BIT_ALIGN>
BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN> operator^(
		const BitSeqWord<MBYTE, BIT_ALIGN>& word_m, const BitSeqWord<MBYTE2, BIT_ALIGN>& word_n);



//////////////////////////////////////////
// VALUE DEFINITION
//////////////////////////////////////////


template <typename MBYTE, BitAlignment BIT_ALIGN>
class BitSeqWord : public BitSeqWordBase<MBYTE> {
private:
	template <typename MBYTE2, BitAlignment BIT_ALIGN2>
	friend class BitSeqWord;

	template <typename T1, typename T2, typename OPERATOR>
	friend BitSeqWord<typename min_width<T1, T2>::value_type, BIT_ALIGN> get_bitwise_operation(
		const BitSeqWord<T1, BIT_ALIGN>&, const BitSeqWord<T2, BIT_ALIGN>&, OPERATOR);	

	MBYTE bits;

public:
	operator MBYTE() const;

	BitSeqWord(
		MBYTE bits = MBYTE(), 
		std::size_t begin = 0, 
		std::size_t end = BitSeqWordBase<MBYTE>::BITS_PER_WORD);

	BitSeqWord(const BitSeqWord& word) = default;

	template <typename MBYTE2>
	BitSeqWord(const BitSeqWord<MBYTE2, BIT_ALIGN>& word);

	BitSeqWord(const BitSeqWord<MBYTE&, BIT_ALIGN>& word);
	//template <BitAlignment BIT_ALIGN2>
	explicit BitSeqWord(const BitSeqWord<MBYTE, invert(BIT_ALIGN)>& word);
	//template <BitAlignment BIT_ALIGN2>
	explicit BitSeqWord(const BitSeqWord<MBYTE&, invert(BIT_ALIGN)>& word);
	
	template <typename MBYTE2>
	BitSeqWord& operator=(const BitSeqWord<MBYTE2, BIT_ALIGN>& word);

	BitSeqWord<MBYTE, BIT_ALIGN> operator~() const;
};


//////////////////////////////////////////
// L-VALUE REFERENCE DEFINITION
//////////////////////////////////////////


template <typename MBYTE, BitAlignment BIT_ALIGN>
class BitSeqWord<MBYTE&, BIT_ALIGN> : public BitSeqWordBase<MBYTE> {
private:
	template <typename MBYTE2, BitAlignment BIT_ALIGN2>
	friend class BitSeqWord;

	template <typename T1, typename T2, typename OPERATOR>
	friend BitSeqWord<typename min_width<T1, T2>::value_type, BIT_ALIGN> get_bitwise_operation(
		const BitSeqWord<T1, BIT_ALIGN>&, const BitSeqWord<T2, BIT_ALIGN>&, OPERATOR);	

	MBYTE& bits;

public:
	operator MBYTE() const;

	inline BitSeqWord(
		MBYTE& bits, 
		std::size_t begin = 0, 
		std::size_t end = BitSeqWordBase<MBYTE>::BITS_PER_WORD);

	BitSeqWord(BitSeqWord&& word) = default;
	template <typename MBYTE2>
	BitSeqWord& operator=(const BitSeqWord<MBYTE2, BIT_ALIGN>& word);

	BitSeqWord<MBYTE, BIT_ALIGN> operator~() const;
};


#include "BitSeqWord.tpp"

#endif