#ifndef BITSEQWORD_H
#define BITSEQWORD_H

#include <climits> // for CHAR_BIT
#include <algorithm> // for max/min
#include <cassert>
#include <limits>
#include <type_traits>


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


template <typename MBYTE>
class BitSeqWord;


//////////////////////////////////////////
// BASE CLASS DEFINITION
//////////////////////////////////////////

template <typename MBYTE>
class BitSeqWordBase {
public:
	typedef char BitIndex_t;
	static constexpr auto BITS_PER_WORD = CHAR_BIT * sizeof(MBYTE);

	static_assert(std::numeric_limits<BitIndex_t>::max() >= BITS_PER_WORD,
		"BitSeqWordBase: insufficient size for bit index type");

protected:
	BitIndex_t bitmargin_lsb, bitmargin_msb;

	inline MBYTE mask() const;

public:
	BitSeqWordBase(BitIndex_t, BitIndex_t);

	inline BitIndex_t size() const;
};

//////////////////////////////////////////
// GLOBAL OVERRIDE DEFINITIONS
//////////////////////////////////////////

template <typename MBYTE, typename NBYTE>
struct min_width {
private:
	typedef typename std::remove_reference<MBYTE>::type MBYTE_VALUE;
	typedef typename std::remove_reference<NBYTE>::type NBYTE_VALUE;
public:
	using value_type = typename std::conditional<
			(BitSeqWordBase<MBYTE_VALUE>::BITS_PER_WORD <= BitSeqWordBase<NBYTE_VALUE>::BITS_PER_WORD),
			MBYTE_VALUE, NBYTE_VALUE
		>::type;
};

template <typename MBYTE, typename NBYTE, typename OPERATOR>
static inline BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type> get_bitwise_operation(
	const BitSeqWord<MBYTE>&, const BitSeqWord<NBYTE>&, OPERATOR);	

template <typename MBYTE, typename NBYTE>
BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type> operator&(
		const BitSeqWord<MBYTE>& word_m, const BitSeqWord<MBYTE>& word_n);
template <typename MBYTE, typename NBYTE>
BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type> operator|(
		const BitSeqWord<MBYTE>& word_m, const BitSeqWord<MBYTE>& word_n);
template <typename MBYTE, typename NBYTE>
BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type> operator^(
		const BitSeqWord<MBYTE>& word_m, const BitSeqWord<MBYTE>& word_n);



//////////////////////////////////////////
// VALUE DEFINITION
//////////////////////////////////////////


template <typename MBYTE>
class BitSeqWord : public BitSeqWordBase<MBYTE> {
private:
	template <typename NBYTE>
	friend class BitSeqWord;

	template <typename T1, typename T2, typename OPERATOR>
	friend BitSeqWord<typename min_width<T1, T2>::value_type> get_bitwise_operation(
		const BitSeqWord<T1>&, const BitSeqWord<T2>&, OPERATOR);	

	MBYTE bits;

	//BitSeqWord& operator<<=(BitIndex_t n);

public:
	inline BitSeqWord(
		MBYTE bits = MBYTE(), 
		std::size_t begin = 0, 
		std::size_t end = BitSeqWordBase<MBYTE>::BITS_PER_WORD);

	template <typename NBYTE>
	BitSeqWord(const BitSeqWord<NBYTE>& word);
	BitSeqWord(const BitSeqWord<MBYTE>& word);
	BitSeqWord(const BitSeqWord<MBYTE&>& word);
	template <typename NBYTE>
	BitSeqWord& operator=(const BitSeqWord<NBYTE>& word);

	inline MBYTE get_bits() const;

	BitSeqWord<MBYTE> operator~() const;
};


//////////////////////////////////////////
// L-VALUE REFERENCE DEFINITION
//////////////////////////////////////////


template <typename MBYTE>
class BitSeqWord<MBYTE&> : public BitSeqWordBase<MBYTE> {
private:
	template <typename NBYTE>
	friend class BitSeqWord;

	template <typename T1, typename T2, typename OPERATOR>
	friend BitSeqWord<typename min_width<T1, T2>::value_type> get_bitwise_operation(
		const BitSeqWord<T1>&, const BitSeqWord<T2>&, OPERATOR);	

	MBYTE& bits;

	// does not require "<<="; shifting disrupts non-target data

public:
	inline BitSeqWord(
		MBYTE& bits, 
		std::size_t begin = 0, 
		std::size_t end = BitSeqWordBase<MBYTE>::BITS_PER_WORD);

	BitSeqWord(BitSeqWord&& word) = default;
	template <typename NBYTE>
	BitSeqWord& operator=(const BitSeqWord<NBYTE>& word);

	inline MBYTE get_bits() const;
	BitSeqWord<MBYTE> operator~() const;
};


#include "BitSeqWord.tpp"

#endif