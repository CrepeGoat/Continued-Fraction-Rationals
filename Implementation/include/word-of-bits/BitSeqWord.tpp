#include "BitTwiddles.hpp"

#include <algorithm>	// max/min
#include <functional>	// bitwise functors


constexpr BitAlignment invert(BitAlignment align) {
	return (align == BitAlignment::LSB ? BitAlignment::MSB : BitAlignment::LSB);
}


//////////////////////////////////////////
// GLOBAL OPERATOR OVERLOADS
//////////////////////////////////////////

template <typename MBYTE, typename MBYTE2, BitAlignment BIT_ALIGN, typename OPERATOR>
BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN> get_bitwise_operation(
	const BitSeqWord<MBYTE, BIT_ALIGN>& word_m,
	const BitSeqWord<MBYTE2, BIT_ALIGN>& word_n,
	OPERATOR op
) {	
	if (word_m.BITS_PER_WORD > word_n.BITS_PER_WORD) {
		return get_bitwise_operation(word_n, word_m, op);
	}

	if (BIT_ALIGN == BitAlignment::LSB) {
		return BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN>(
			op(
				word_m.bits,
				shift_left(word_n.bits,
					word_m.bit_margin_lsb - word_n.bit_margin_lsb)
			),
			word_m.bit_margin_lsb,
			std::min<typename max_width<
					typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t,
					typename BitSeqWord<MBYTE2, BIT_ALIGN>::BitIndex_t>::type
				>(word_m.bit_non_msbmargin(), word_m.bit_margin_lsb + word_n.size())
		);
	} else {
		return BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN>(
			op(
				word_m.bits,
				shift_left(word_n.bits,
					word_m.bit_non_msbmargin() - word_n.bit_non_msbmargin())
			),
			std::max<typename max_width<
					typename BitSeqWord<MBYTE, BIT_ALIGN>::BitIndex_t,
					typename BitSeqWord<MBYTE2, BIT_ALIGN>::BitIndex_t>::type
				>(word_m.bit_margin_lsb, word_m.bit_non_msbmargin() - word_n.size()),
			word_m.bit_non_msbmargin()
		);
	}
}



template <typename MBYTE, typename MBYTE2, BitAlignment BIT_ALIGN>
BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN> operator&(
	const BitSeqWord<MBYTE, BIT_ALIGN>& word_m, const BitSeqWord<MBYTE2, BIT_ALIGN>& word_n)
{
	return get_bitwise_operation(word_m, word_n, std::bit_and<>());
}
template <typename MBYTE, typename MBYTE2, BitAlignment BIT_ALIGN>
BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN> operator|(
	const BitSeqWord<MBYTE, BIT_ALIGN>& word_m, const BitSeqWord<MBYTE2, BIT_ALIGN>& word_n)
{
	return get_bitwise_operation(word_m, word_n, std::bit_or<>());
}
template <typename MBYTE, typename MBYTE2, BitAlignment BIT_ALIGN>
BitSeqWord<typename min_width<MBYTE, MBYTE2>::type, BIT_ALIGN> operator^(
	const BitSeqWord<MBYTE, BIT_ALIGN>& word_m, const BitSeqWord<MBYTE2, BIT_ALIGN>& word_n)
{
	return get_bitwise_operation(word_m, word_n, std::bit_xor<>());
}

//////////////////////////////////////////
// BASE CLASS IMPLEMENTATION
//////////////////////////////////////////

template <typename MBYTE>
MBYTE BitSeqWordBase<MBYTE>::mask() const {
	return ~(~MBYTE(0) << size()) << bit_margin_lsb;
	// TODO fix this to work
	//return mask<MBYTE>(bit_margin_lsb, size());
}


template <typename MBYTE>
typename BitSeqWordBase<MBYTE>::BitIndex_t
BitSeqWordBase<MBYTE>::bit_non_lsbmargin() const {
	return this->BITS_PER_WORD - bit_margin_lsb;
}
template <typename MBYTE>
typename BitSeqWordBase<MBYTE>::BitIndex_t
BitSeqWordBase<MBYTE>::bit_non_msbmargin() const {
	return this->BITS_PER_WORD - bit_margin_msb;
}

/*
template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWordBase<MBYTE>::BitIndex_t BitSeqWordBase<MBYTE>::bit_margin_front() const {
	return (BIT_ALIGN == BitAlignment::LSB ? bit_margin_lsb : bit_margin_msb);
}
template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWordBase<MBYTE>::BitIndex_t BitSeqWordBase<MBYTE>::bit_margin_back() const {
	return (BIT_ALIGN == BitAlignment::LSB ? bit_margin_msb : bit_margin_lsb);
}

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWordBase<MBYTE>::BitIndex_t BitSeqWordBase<MBYTE>::bit_nonmargin_front() const {
	return (BIT_ALIGN == BitAlignment::LSB ? bit_non_lsbmargin() : bit_non_msbmargin());
}
template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWordBase<MBYTE>::BitIndex_t BitSeqWordBase<MBYTE>::bit_nonmargin_back() const {
	return (BIT_ALIGN == BitAlignment::LSB ? bit_non_msbmargin() : bit_non_lsbmargin());
}
//*/

template <typename MBYTE>
BitSeqWordBase<MBYTE>::BitSeqWordBase(
	typename BitSeqWordBase<MBYTE>::BitIndex_t begin,
	typename BitSeqWordBase<MBYTE>::BitIndex_t fromend
	) : bit_margin_lsb(begin),
		bit_margin_msb(fromend)
	{}

template <typename MBYTE>
typename BitSeqWordBase<MBYTE>::BitIndex_t BitSeqWordBase<MBYTE>::size() const {
	return this->BITS_PER_WORD - (bit_margin_lsb + bit_margin_msb); 
}








//////////////////////////////////////////
// VALUE DEFINITION & IMPLEMENTATION
//////////////////////////////////////////

/*

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN>& BitSeqWord<MBYTE, BIT_ALIGN>::operator<<=(
	typename BitSeqWordBase<MBYTE>::BitIndex_t n)
{
	shift_fwd(bits, n);
	if (n >= 0) {
		bit_margin_lsb = std::min(bit_margin_lsb + n, this->BITS_PER_WORD);
		bit_margin_msb = std::max(bit_margin_msb - n, 0);
	} else {
		bit_margin_lsb = std::max(bit_margin_lsb + n, 0);
		bit_margin_msb = std::min(bit_margin_msb - n, this->BITS_PER_WORD);
	}
	return *this;
}
//*/


template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN>::BitSeqWord(
		MBYTE machine_word,
		std::size_t begin, 
		std::size_t end
	) : BitSeqWordBase<MBYTE>(
			begin,
			(end < this->BITS_PER_WORD)
				? this->BITS_PER_WORD - end : 0
			),
		bits(machine_word)
	{}

template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
BitSeqWord<MBYTE, BIT_ALIGN>::BitSeqWord(const BitSeqWord<MBYTE2, BIT_ALIGN>& word)
	// Need to shift to least-significant bits, to avoid overflow in type conversion
	:	BitSeqWordBase<MBYTE>(
			(BIT_ALIGN == BitAlignment::LSB || word.size() > this->BITS_PER_WORD)
				? 0 : this->BITS_PER_WORD - word.size(),
			(BIT_ALIGN == BitAlignment::MSB || word.size() > this->BITS_PER_WORD)
				? 0 : this->BITS_PER_WORD - word.size()
		),
		bits((BIT_ALIGN == BitAlignment::LSB)
			? word.bits >> word.bit_margin_lsb
			: shift_left<typename max_width<MBYTE, MBYTE2>::type>(
				word.bits, this->BITS_PER_WORD - word.bit_non_msbmargin())
			//: this->BITS_PER_WORD <= word.bit_non_msbmargin()
			//	? word.bits >> (word.bit_non_msbmargin() - this->BITS_PER_WORD)
			//	: MBYTE(word.bits) << (this->BITS_PER_WORD - word.bit_non_msbmargin())
		)
	{}

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN>::BitSeqWord(const BitSeqWord<MBYTE&, BIT_ALIGN>& word)
	// Can simply member-wise copy with same word type
	: BitSeqWordBase<MBYTE>(word.bit_margin_lsb, word.bit_margin_msb),
		bits(word.bits)
	{}

template <typename MBYTE, BitAlignment BIT_ALIGN>
//template <BitAlignment BIT_ALIGN2>
BitSeqWord<MBYTE, BIT_ALIGN>::BitSeqWord(const BitSeqWord<MBYTE, invert(BIT_ALIGN)>& word)
	// Can simply member-wise copy with same word type
	:	BitSeqWordBase<MBYTE>(word.bit_margin_lsb, word.bit_margin_msb),
		bits(word.bits)
	{}
template <typename MBYTE, BitAlignment BIT_ALIGN>
//template <BitAlignment BIT_ALIGN2>
BitSeqWord<MBYTE, BIT_ALIGN>::BitSeqWord(const BitSeqWord<MBYTE&, invert(BIT_ALIGN)>& word)
	// Can simply member-wise copy with same word type
	: BitSeqWordBase<MBYTE>(word.bit_margin_lsb, word.bit_margin_msb),
		bits(word.bits)
	{}


template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
BitSeqWord<MBYTE, BIT_ALIGN>& BitSeqWord<MBYTE, BIT_ALIGN>::operator=(
	const BitSeqWord<MBYTE2, BIT_ALIGN>& word
	)
{
	if (word.BITS_PER_WORD <= this->BITS_PER_WORD
			|| word.bit_non_msbmargin() <= this->BITS_PER_WORD) {
		// Can simply member-wise copy with same word type
		this->bits = word.bits;
		this->bit_margin_lsb = word.bit_margin_lsb;
		this->bit_margin_msb = this->BITS_PER_WORD - word.bit_non_msbmargin();
	} else {
		// Need to shift to least-significant bits, to avoid overflow in type conversion
		this->bit_margin_lsb = (BIT_ALIGN == BitAlignment::LSB || word.size() > this->BITS_PER_WORD)
				? 0 : this->BITS_PER_WORD - word.size();
		this->bit_margin_msb = (BIT_ALIGN == BitAlignment::MSB || word.size() > this->BITS_PER_WORD)
				? 0 : this->BITS_PER_WORD - word.size();
		
		this->bits = (BIT_ALIGN == BitAlignment::LSB)
			? word.bits >> word.bit_margin_lsb
			: shift_left<typename max_width<MBYTE, MBYTE2>::type>(
					word.bits, this->BITS_PER_WORD - word.bit_non_msbmargin());
	}
	return *this;
}



template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN>::operator MBYTE() const {
	return (bits >> this->bit_margin_lsb) & mask<MBYTE>(0, this->size()); 
}

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN> BitSeqWord<MBYTE, BIT_ALIGN>::operator~() const {
	return BitSeqWord<MBYTE, BIT_ALIGN>(
		~bits, this->bit_margin_lsb, this->bit_non_msbmargin());
}






//////////////////////////////////////////
// L-VALUE REFERENCE DEF & IMPLEMENTATION
//////////////////////////////////////////

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE&, BIT_ALIGN>::BitSeqWord(
		MBYTE& machine_word, 
		std::size_t begin, 
		std::size_t end
	) : BitSeqWordBase<MBYTE>(begin, (end < this->BITS_PER_WORD) 
			? this->BITS_PER_WORD - end : 0),
		bits(machine_word)
	{}


template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
BitSeqWord<MBYTE&, BIT_ALIGN>& BitSeqWord<MBYTE&, BIT_ALIGN>::operator=(
	const BitSeqWord<MBYTE2, BIT_ALIGN>& word
	)
{	
	const auto word_leftbitshift = (BIT_ALIGN == BitAlignment::LSB)
		? this->bit_margin_lsb - word.bit_margin_lsb
		: this->bit_non_msbmargin() - word.bit_non_msbmargin();

	const MBYTE word_bits = (this->BITS_PER_WORD <= word.BITS_PER_WORD)
		? MBYTE(shift_left(word.bits, word_leftbitshift))
		: shift_left(MBYTE(word.bits), word_leftbitshift);

	if (this->size() > word.size()) {
		if (BIT_ALIGN == BitAlignment::LSB) {
			this->bit_margin_msb = this->bit_non_lsbmargin() - word.size();
		} else {
			this->bit_margin_lsb = this->bit_non_msbmargin() - word.size();
		}
	}
	apply_bits(word_bits, this->bits, this->mask());
	return *this;
}


template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE&, BIT_ALIGN>::operator MBYTE() const {
	return (bits >> this->bit_margin_lsb) & mask<MBYTE>(0, this->size()); 
}

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN> BitSeqWord<MBYTE&, BIT_ALIGN>::operator~() const {
	return BitSeqWord<MBYTE, BIT_ALIGN>(
		~bits, this->bit_margin_lsb, this->bit_non_msbmargin());
}


