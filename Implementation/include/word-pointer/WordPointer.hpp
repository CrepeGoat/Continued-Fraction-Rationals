#ifndef WORDPOINTER_HPP
#define WORDPOINTER_HPP

#include <cstddef>	// std::ptrdiff_t
#include <cstdint>	// uint8_t


/*
 *	CLASS - WordPointer
 *
 * FUNCTIONALITY
 *  - merely an abstraction-layer wrapper for an existing byte-iterator type
 *    - abstracts incrementation based on forward / backward orientation
 *    - abstracts/implements the dereferencing of consecutive bytes as a larger word
 *
 *  -> allows for iteration through sequence of bytes in either direction
 *     while evaluating bytes in various machine-word-size chunks
 *
 * FILE ORGANIZATION
 *  - definitions
 *    - forward-oriented -> "WordPointer.tpp"
 *    - backward-oriented -> "WordPointer_reverse.tpp"
 *        (implemented as template specialization)
 *
 * POSSIBLE ADDITIONS
 *  - expand to work more effectively with `std::deque` iterators
 *    - checking `if (*this - ptr) >= sizeof(MWORD)` is not sufficient,
 *        due to discontinuous data layout of deques  
 *    - -> make a `has_next_mword<MWORD>()` function
 *
 */




template <typename BYTE_PTR_T, bool BYTES_FWD>
class WordPointer {
private:
	BYTE_PTR_T b_ptr;

public:
	// These methods are "overrides" that take into account byte-direction
	// v
	WordPointer& operator++();
	WordPointer operator++(int);
	
	WordPointer operator+(std::size_t);
	WordPointer& operator+=(std::size_t);

	
	WordPointer& operator--();
	WordPointer operator--(int);

	WordPointer operator-(std::size_t);
	WordPointer& operator-=(std::size_t);

	std::ptrdiff_t operator-(const BYTE_PTR_T&);
	// ^

	// These methods dereference bytes in chunks as whole machine words
	// v
	uint8_t& operator*();
	
	template <typename MWORD>
	MWORD& deref();
	// ^
};

template <typename BYTE_PTR_T>
class WordPointer<BYTE_PTR_T, false>;


#include "WordPointer.tpp"

#endif
