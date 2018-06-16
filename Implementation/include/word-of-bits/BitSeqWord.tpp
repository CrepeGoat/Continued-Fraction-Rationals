#include "BitTwiddles.hpp"

#include <algorithm>	// max/min
#include <functional>	// bitwise functors


constexpr BitAlignment invert(BitAlignment align) {
	return (align == BitAlignment::LSB ? BitAlignment::MSB : BitAlignment::LSB);
}


//////////////////////////////////////////
// BASE CLASS IMPLEMENTATION
//////////////////////////////////////////

template <typename MBYTE>
MBYTE BitSeqWord_Base<MBYTE>::mask() const {
	return ~(~MBYTE(0) << size()) << bit_margin_lsb;
	// TODO fix this to work
	//return mask<MBYTE>(bit_margin_lsb, size());
}


template <typename MBYTE>
typename BitSeqWord_Base<MBYTE>::BitIndex_t
BitSeqWord_Base<MBYTE>::bit_non_lsbmargin() const {
	return this->BITS_PER_WORD - bit_margin_lsb;
}
template <typename MBYTE>
typename BitSeqWord_Base<MBYTE>::BitIndex_t
BitSeqWord_Base<MBYTE>::bit_non_msbmargin() const {
	return this->BITS_PER_WORD - bit_margin_msb;
}

/*
template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord_Base<MBYTE>::BitIndex_t BitSeqWord_Base<MBYTE>::bit_margin_front() const {
	return (BIT_ALIGN == BitAlignment::LSB ? bit_margin_lsb : bit_margin_msb);
}
template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord_Base<MBYTE>::BitIndex_t BitSeqWord_Base<MBYTE>::bit_margin_back() const {
	return (BIT_ALIGN == BitAlignment::LSB ? bit_margin_msb : bit_margin_lsb);
}

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord_Base<MBYTE>::BitIndex_t BitSeqWord_Base<MBYTE>::bit_nonmargin_front() const {
	return (BIT_ALIGN == BitAlignment::LSB ? bit_non_lsbmargin() : bit_non_msbmargin());
}
template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord_Base<MBYTE>::BitIndex_t BitSeqWord_Base<MBYTE>::bit_nonmargin_back() const {
	return (BIT_ALIGN == BitAlignment::LSB ? bit_non_msbmargin() : bit_non_lsbmargin());
}
//*/

template <typename MBYTE>
BitSeqWord_Base<MBYTE>::BitSeqWord_Base(
	typename BitSeqWord_Base<MBYTE>::BitIndex_t begin,
	typename BitSeqWord_Base<MBYTE>::BitIndex_t fromend
	) : bit_margin_lsb(begin),
		bit_margin_msb(fromend)
	{}

template <typename MBYTE>
typename BitSeqWord_Base<MBYTE>::BitIndex_t BitSeqWord_Base<MBYTE>::size() const {
	return this->BITS_PER_WORD - (bit_margin_lsb + bit_margin_msb); 
}








//////////////////////////////////////////
// VALUE DEFINITION & IMPLEMENTATION
//////////////////////////////////////////

/*

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord_SubBase<MBYTE, BIT_ALIGN>& BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::operator<<=(
	typename BitSeqWord_Base<MBYTE>::BitIndex_t n)
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
BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::BitSeqWord_SubBase(
		MBYTE machine_word,
		std::size_t begin, 
		std::size_t end
	) : BitSeqWord_Base<MBYTE>(
			begin,
			(end < this->BITS_PER_WORD)
				? this->BITS_PER_WORD - end
				: 0
			),
		bits(machine_word)
	{}

template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::BitSeqWord_SubBase(
		const BitSeqWord_SubBase<MBYTE2, BIT_ALIGN>& word
	)
	// Need to shift to least-significant bits, to avoid overflow in type conversion
	:	BitSeqWord_Base<MBYTE>(
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
BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::BitSeqWord_SubBase(
		const BitSeqWord_SubBase<MBYTE&, BIT_ALIGN>& word
	)
	// Can simply member-wise copy with same word type
	: BitSeqWord_Base<MBYTE>(word.bit_margin_lsb, word.bit_margin_msb),
		bits(word.bits)
	{}

template <typename MBYTE, BitAlignment BIT_ALIGN>
//template <BitAlignment BIT_ALIGN2>
BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::BitSeqWord_SubBase(
		const BitSeqWord_SubBase<MBYTE, invert(BIT_ALIGN)>& word
	)
	// Can simply member-wise copy with same word type
	:	BitSeqWord_Base<MBYTE>(word.bit_margin_lsb, word.bit_margin_msb),
		bits(word.bits)
	{}
template <typename MBYTE, BitAlignment BIT_ALIGN>
//template <BitAlignment BIT_ALIGN2>
BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::BitSeqWord_SubBase(
		const BitSeqWord_SubBase<MBYTE&, invert(BIT_ALIGN)>& word
	)
	// Can simply member-wise copy with same word type
	: BitSeqWord_Base<MBYTE>(word.bit_margin_lsb, word.bit_margin_msb),
		bits(word.bits)
	{}


template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
BitSeqWord_SubBase<MBYTE, BIT_ALIGN>&
BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::operator=(
	const BitSeqWord_SubBase<MBYTE2, BIT_ALIGN>& word
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
		this->bit_margin_lsb = 
			(BIT_ALIGN == BitAlignment::LSB || word.size() > this->BITS_PER_WORD)
			? 0 : this->BITS_PER_WORD - word.size();
		this->bit_margin_msb = 
			(BIT_ALIGN == BitAlignment::MSB || word.size() > this->BITS_PER_WORD)
				? 0 : this->BITS_PER_WORD - word.size();
		
		this->bits = (BIT_ALIGN == BitAlignment::LSB)
			? word.bits >> word.bit_margin_lsb
			: shift_left<typename max_width<MBYTE, MBYTE2>::type>(
					word.bits, this->BITS_PER_WORD - word.bit_non_msbmargin());
	}
	return *this;
}






//////////////////////////////////////////
// L-VALUE REFERENCE DEF & IMPLEMENTATION
//////////////////////////////////////////

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord_SubBase<MBYTE&, BIT_ALIGN>::BitSeqWord_SubBase(
		MBYTE& machine_word, 
		std::size_t begin, 
		std::size_t end
	) : BitSeqWord_Base<MBYTE>(begin, (end < this->BITS_PER_WORD) 
			? this->BITS_PER_WORD - end : 0),
		bits(machine_word)
	{}


template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
BitSeqWord_SubBase<MBYTE&, BIT_ALIGN>& BitSeqWord_SubBase<MBYTE&, BIT_ALIGN>::operator=(
	const BitSeqWord_SubBase<MBYTE2, BIT_ALIGN>& word
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






//////////////////////////////////////////
// DERIVED IMPLEMENTATIONS
//////////////////////////////////////////

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN>::operator typename std::remove_reference<MBYTE>::type () const {
	return mask<typename std::remove_reference<MBYTE>::type>(0, this->size())
		& (this->bits >> this->bit_margin_lsb); 
}

template <typename MBYTE, BitAlignment BIT_ALIGN>
BitSeqWord<typename std::remove_reference<MBYTE>::type, BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN>::operator~() const
{
	return BitSeqWord<typename std::remove_reference<MBYTE>::type, BIT_ALIGN>(
		~this->bits, this->bit_margin_lsb, this->bit_non_msbmargin());
}




template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2, typename OP_T>
BitSeqWord<typename std::remove_reference<typename min_width<MBYTE, MBYTE2>
	::type>::type, BIT_ALIGN>
BitSeqWord<MBYTE, BIT_ALIGN>::get_binary_operation(
	const BitSeqWord<MBYTE2, BIT_ALIGN>& rhs,
	OP_T op
) const {
	if (this->BITS_PER_WORD > rhs.BITS_PER_WORD) {
		return rhs.get_binary_operation(*this, op);
	}

	using RETURN_TYPE = BitSeqWord<typename std::remove_reference<typename
			min_width<MBYTE, MBYTE2>::type>::type, BIT_ALIGN>;

	if /*constexpr*/ (BIT_ALIGN == BitAlignment::LSB)
		return RETURN_TYPE(
			op(
				this->bits,
				shift_left(rhs.bits,
					this->bit_margin_lsb - rhs.bit_margin_lsb)
			),
			this->bit_margin_lsb,
			std::min<typename max_width<
					typename BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::BitIndex_t,
					typename BitSeqWord_SubBase<MBYTE2, BIT_ALIGN>::BitIndex_t>::type
				>(this->bit_non_msbmargin(), this->bit_margin_lsb + rhs.size())
		);
	else
		return RETURN_TYPE(
			op(
				this->bits,
				shift_left(rhs.bits,
					this->bit_non_msbmargin() - rhs.bit_non_msbmargin())
			),
			std::max<typename max_width<
					typename BitSeqWord_SubBase<MBYTE, BIT_ALIGN>::BitIndex_t,
					typename BitSeqWord_SubBase<MBYTE2, BIT_ALIGN>::BitIndex_t>::type
				>(this->bit_margin_lsb, this->bit_non_msbmargin() - rhs.size()),
			this->bit_non_msbmargin()
		);
}



template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
auto BitSeqWord<MBYTE, BIT_ALIGN>::operator&(
	const BitSeqWord<MBYTE2, BIT_ALIGN>& rhs
) {
	return this->get_binary_operation(rhs, std::bit_and<>());
}
template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
auto BitSeqWord<MBYTE, BIT_ALIGN>::operator|(
	const BitSeqWord<MBYTE2, BIT_ALIGN>& rhs
) {
	return this->get_binary_operation(rhs, std::bit_or<>());
}
template <typename MBYTE, BitAlignment BIT_ALIGN>
template <typename MBYTE2>
auto BitSeqWord<MBYTE, BIT_ALIGN>::operator^(
	const BitSeqWord<MBYTE2, BIT_ALIGN>& rhs
) {
	return this->get_binary_operation(rhs, std::bit_xor<>());
}

