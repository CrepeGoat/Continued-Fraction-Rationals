#include <cstddef>
#include <cstdlib>
#include <climits>
#include <algorithm>
#include <cassert>

#include "BitTwiddles.h"

const bool LITTLE = true;
const bool BIG = false;

static const bool system_endian_runtime() {
	constexpr unsigned long long x(1);
	return (*((unsigned char*)&x) == 1) ? LITTLE : BIG;
}

template <typename MBYTE>
static void apply_bits(MBYTE bits_from, MBYTE& bits_to, MBYTE mask) {
	bits_to ^= (bits_from^bits_to) & mask;
		// for all mask[i]==1, bits_to[i] -> bits_from[i]
		// for all mask[i]==0, bits_to[i] -> bits_to[i]
}

// Generic Methods for functionality independent of endianness
template <bool ENDIAN>
template <bool DIRECTION, typename MBYTE>
MBYTE BitSequence<ENDIAN>::fwd_shift(MBYTE value, std::ptrdiff_t n) {
	return std::abs(n) >= CHAR_BIT*sizeof(MBYTE) ? 0:
		((ENDIAN==LITTLE) == (DIRECTION==FWD) == (n>=0))
		? value<<std::abs(n)
		: value>>std::abs(n);
}

template <bool ENDIAN>
template <bool DIRECTION>
std::size_t BitSequence<ENDIAN>::fwd_subindex() const {
	return (ENDIAN==LITTLE)==(DIRECTION==FWD)
			? subindex : CHAR_BIT-1-subindex;
}
template <bool ENDIAN>
template <bool DIRECTION>
void BitSequence<ENDIAN>::set_fwd_subindex(std::size_t n) {
	subindex = (ENDIAN==LITTLE)==(DIRECTION==FWD) ? n : CHAR_BIT-1-n;
}

template <bool ENDIAN>
template <bool DIRECTION, typename MBYTE>
MBYTE& BitSequence<ENDIAN>::fwd_mbyte() {
	return *(MBYTE*)((DIRECTION==FWD) ? p_index : p_index - (sizeof(MBYTE)-1));
}

template <bool ENDIAN>
std::ptrdiff_t BitSequence<ENDIAN>::bytes_left() const {
	return p_end-p_index;
}
template <bool ENDIAN>
template <bool DIRECTION>
void BitSequence<ENDIAN>::fwd_inc_p_index(std::size_t n) {
	if (DIRECTION==FWD)
		p_index+=n;
	else
		p_index-=n;
}

template <bool ENDIAN>
template <bool DIRECTION>
std::size_t BitSequence<ENDIAN>::bytes_covered_by_next_bits(std::size_t bitcount) const {
	return (bitcount == 0) ? 0 :
		(bitcount + fwd_subindex<DIRECTION>() + CHAR_BIT-1) / CHAR_BIT;
}

template <bool ENDIAN>
template <bool DIRECTION>
void BitSequence<ENDIAN>::fwd_skip_next(std::size_t bitcount) {
	fwd_inc_p_index<DIRECTION>((fwd_subindex<DIRECTION>()+bitcount)/CHAR_BIT);
	set_fwd_subindex<DIRECTION>((fwd_subindex<DIRECTION>()+bitcount)%CHAR_BIT);
}








// Constructors
template <bool ENDIAN>
BitSequence<ENDIAN>::BitSequence() : p_index(NULL), p_end(NULL) {
	assert(sizeof(unsigned long long) == sizeof(unsigned char)
			|| ENDIAN == system_endian_runtime()
		);
}

template <bool ENDIAN>
BitSequence<ENDIAN>::BitSequence(byte* begin, byte* end) :
		subindex(ENDIAN==LITTLE ? 0:CHAR_BIT-1),
		p_index(begin),
		p_end(std::max(begin,end))
{
	assert(sizeof(unsigned long long) == sizeof(unsigned char)
			|| ENDIAN == system_endian_runtime()
		);
}
//template <bool ENDIAN>
//BitSequence<ENDIAN>::BitSequence(const BitSequence& rhs) :
//		subindex(rhs.subindex),
//		p_index(rhs.p_index),
//		p_end(rhs.p_end) {}
template <bool ENDIAN>
void BitSequence<ENDIAN>::init(byte* begin, byte* end) {
	subindex = (ENDIAN==LITTLE ? 0:CHAR_BIT-1);
	p_index = begin;
	p_end = end;
}


template <bool ENDIAN>
BitSequence<ENDIAN>::operator bool() {
	return has_next();
}
template <bool ENDIAN>
std::size_t BitSequence<ENDIAN>::bits_left() const {
	return CHAR_BIT*sizeof(byte)*bytes_left()-fwd_subindex<FWD>();
}
template <bool ENDIAN>
bool BitSequence<ENDIAN>::has_next() const {
	return bytes_left() > 0;
}
template <bool ENDIAN>
bool BitSequence<ENDIAN>::has_next(std::size_t bitcount) const {
	return bitcount <= bits_left();
}
template <bool ENDIAN>
void BitSequence<ENDIAN>::skip_next(std::size_t bitcount) {
	fwd_skip_next<FWD>(std::min(bits_left(), bitcount));
}

template <bool ENDIAN>
bool BitSequence<ENDIAN>::peek_next() const {
	if (has_next()) {
		return (((byte(1)<<subindex) & *p_index) != 0);
	}
	return bool();
}
template <bool ENDIAN>
bool BitSequence<ENDIAN>::get_next() {
	bool bit;
	if (has_next()) {
		bit = (((byte(1)<<subindex) & *p_index) != 0);
		fwd_skip_next<FWD>(1);
	}
	return bit;
}
template <bool ENDIAN>
void BitSequence<ENDIAN>::set_next(bool bit) {
	if (has_next()) {
		apply_bits<byte>(byte(bit)<<subindex, *p_index, 1<<subindex);
		fwd_skip_next<FWD>(1);
	}
}


template <bool ENDIAN>
inline BitSequence<ENDIAN>& BitSequence<ENDIAN>::operator>>(bool& bit) {
	bit = get_next();
	return *this;
}
template <bool ENDIAN>
inline BitSequence<ENDIAN>& BitSequence<ENDIAN>::operator<<(bool bit) {
	set_next(bit);
	return *this;
}








/* FUNCTION		 get
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * DESCRIPTION
 *  This method returns the number of sequential following bits are set to "bit".
 *
 *  The bits are checked in blocks of m bytes, leveraging larger register sizes
 *  to reduce the number of bitwise operations, and thus the check time.
 */
template <bool ENDIAN>
template <typename MBYTE>
MBYTE BitSequence<ENDIAN>::first_1bit(MBYTE value) {
	return (ENDIAN == LITTLE) ? lsb(value) : msb(value);
}


template <bool ENDIAN>
template <typename MBYTE>
bool BitSequence<ENDIAN>::get_streak_loopfunc(bool bit, std::size_t& bitcount) {
	// Saves position of first non-"bit" bit value
	const std::size_t len_advance = std::min(bitcount,
			bit_pos<MBYTE>(first_1bit<MBYTE>(		// position of first_1bit of
				(-MBYTE(bit) ^ fwd_mbyte<FWD,MBYTE>()) &	// (non-)inverted MBYTE
				fwd_shift<FWD,MBYTE>(-1, fwd_subindex<FWD>())	// w/ high-pass bit mask
			)) - fwd_subindex<FWD>()			// minus current bit position
		);
	bitcount -= len_advance;
	fwd_skip_next<FWD>(len_advance);
	return (fwd_subindex<FWD>() == 0 && len_advance > CHAR_BIT*(sizeof(MBYTE)-1)
			// ^ check if search hit end of mbyte
			&& bitcount > 0 // and if there are more bits to check
		);
}

template <bool ENDIAN>
template <std::size_t MAX_M>
std::size_t BitSequence<ENDIAN>::get_streak(bool bit, std::size_t max_bitcount) {
	max_bitcount = std::min(max_bitcount, bits_left());
	std::size_t bitcount = max_bitcount;

	#define get_streak_innermacro(T,SIZEOF_T_prev) \
		if (sizeof(T) <= std::min(MAX_M, SIZEOF_T_prev-1)) { \
			while (bytes_covered_by_next_bits<FWD>(bitcount) >= sizeof(T)) { \
				if (!get_streak_loopfunc<T>(bit, bitcount)) { \
					goto RETURN; \
				} \
			} \
		}
	get_streak_innermacro(unsigned long long,std::size_t(-1))
	get_streak_innermacro(unsigned long,sizeof(unsigned long long))
	get_streak_innermacro(unsigned int,sizeof(unsigned long))
	get_streak_innermacro(unsigned short,sizeof(unsigned int))
	get_streak_innermacro(byte,sizeof(unsigned short))
	RETURN:
	#undef get_streak_innermacro

	return max_bitcount - bitcount;
}

/* FUNCTION		 set
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * DESCRIPTION
 *  This method applies 'bitcount' copies of 'bit' to the bit sequence.
 *
 *  The bits are copied in blocks of m bytes, leveraging larger register sizes
 *  to reduce the number of bitwise operations, and thus the copy time.
 */
template <bool ENDIAN>
template <typename MBYTE>
void BitSequence<ENDIAN>::set_streak_loopfunc(bool bit, std::size_t& bitcount) {
	//if (bitcount<=0) return false;
	const std::size_t len_advance = std::min(bitcount,
			CHAR_BIT*sizeof(MBYTE)-fwd_subindex<FWD>()
		);
	apply_bits<MBYTE>(-bit, fwd_mbyte<FWD,MBYTE>(), fwd_shift<FWD,MBYTE>(
			~fwd_shift<FWD,MBYTE>(-1, len_advance), fwd_subindex<FWD>()
		));
	bitcount -= len_advance;
	fwd_skip_next<FWD>(len_advance);
	//return true;
}
template <bool ENDIAN>
template <std::size_t MAX_M>
void BitSequence<ENDIAN>::set_streak(bool bit, std::size_t bitcount) {
	// Adjusts bitcount to fit stream range
	bitcount = std::min(bitcount, bits_left());
	// Passes bits in as large of byte blocks as possible
	#define set_streak_innermacro(T,SIZEOF_T_prev) \
		if (sizeof(T) <= MAX_M && sizeof(T) < SIZEOF_T_prev) { \
			while (sizeof(T) <= bytes_covered_by_next_bits<FWD>(bitcount)) { \
				set_streak_loopfunc<T>(bit, bitcount); \
			} \
		}
	set_streak_innermacro(unsigned long long,std::size_t(-1))
	set_streak_innermacro(unsigned long,sizeof(unsigned long long))
	set_streak_innermacro(unsigned int,sizeof(unsigned long))
	set_streak_innermacro(unsigned short,sizeof(unsigned int))
	#undef set_streak_innermacro
	// Passes remaining bits by single byte
	while (bitcount > 0) {
		set_streak_loopfunc<byte>(bit, bitcount);
	}
}

template <bool ENDIAN>
template <typename MBYTE>
void BitSequence<ENDIAN>::set_from_int(MBYTE source, std::size_t bitcount) {
	bitcount = std::min(bitcount, CHAR_BIT*sizeof(MBYTE));
	BitSequence<ENDIAN> bseq2((byte*)&source, (byte*)(&source+sizeof(MBYTE)));
	if (ENDIAN == BIG) {
		bseq2.skip_next(CHAR_BIT*sizeof(MBYTE) - bitcount);
	}
	set_from(bseq2, bitcount);
}

template <bool ENDIAN>
template <typename MBYTE>
void BitSequence<ENDIAN>::get_to_int(MBYTE& dest, std::size_t bitcount) {
	bitcount = std::min(bitcount, CHAR_BIT*sizeof(MBYTE));
	BitSequence<ENDIAN> bseq2((byte*)&dest, (byte*)(&dest+sizeof(MBYTE)));
	if (ENDIAN == BIG) {
		bseq2.skip_next(CHAR_BIT*sizeof(MBYTE) - bitcount);
	}
	bseq2.set_from(*this, bitcount);
}

/* FUNCTION		 set_from
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * DESCRIPTION
 *  This method applies 'bitcount' bits from the provided 'source' bit sequence
 *  to '*this' sequence.
 *
 *  The bits are copied in blocks of m bytes, leveraging larger register sizes
 *  to reduce the number of bitwise operations, and thus the copy time.
 *
 *  The order in which the bytes are copied (forwards/backwards) is chosen to
 *  eliminate the possibility of copy errors in the cases where the two
 *  bit sequences overlap.
 */
template <bool ENDIAN>
template <bool DIRECTION,typename MBYTE>
void BitSequence<ENDIAN>::set_from_loopfunc(
		BitSequence& source,
		std::size_t& bitcount
	) {
	const std::size_t len_advance = std::min<std::size_t>(
			bitcount, CHAR_BIT*sizeof(MBYTE) - std::max(
				fwd_subindex<DIRECTION>(),
				source.fwd_subindex<DIRECTION>()
			)
		);
	// Applies bits in BLOCK of bytes
	apply_bits<MBYTE>(
			// Source of BLOCK copy
			fwd_shift<DIRECTION,MBYTE>(
				source.fwd_mbyte<DIRECTION,MBYTE>(),
				fwd_subindex<DIRECTION>()-source.fwd_subindex<DIRECTION>()
			),
			// Target of BLOCK copy
			fwd_mbyte<DIRECTION,MBYTE>(),
			// Mask for copy
			fwd_shift<DIRECTION,MBYTE>(
				~fwd_shift<DIRECTION,MBYTE>(-1,len_advance),
				fwd_subindex<DIRECTION>()
			)
		);

	bitcount -= len_advance;
	fwd_skip_next<DIRECTION>(len_advance);
	source.fwd_skip_next<DIRECTION>(len_advance);
}

template <bool ENDIAN>
template <bool DIRECTION, std::size_t MAX_M>
void BitSequence<ENDIAN>::set_from_innerfunc(BitSequence& source, std::size_t bitcount) {
	// Passes bits in as many m-byte blocks as possible
	//  without exceeding streams' byte-assignment ranges
	// (though assigning to external bytes w/ mask would not alter data,
	//  it could result in race issues if simultaneously assigning
	//  in another program/stream)
	#define set_from_innermacro(T,SIZEOF_T_prev) \
		if (sizeof(T) <= MAX_M && sizeof(T) < SIZEOF_T_prev) { \
			while (sizeof(T) <= std::min( \
				bytes_covered_by_next_bits<DIRECTION>(bitcount), \
				source.bytes_covered_by_next_bits<DIRECTION>(bitcount) \
			)) { \
				set_from_loopfunc<DIRECTION,T>(source, bitcount); \
			} \
		}
	set_from_innermacro(unsigned long long,std::size_t(-1))
	set_from_innermacro(unsigned long,sizeof(unsigned long long))
	set_from_innermacro(unsigned int,sizeof(unsigned long))
	set_from_innermacro(unsigned short,sizeof(unsigned int))
	#undef set_from_innermacro
	// Passes remaining bits in byte blocks
	while (bitcount > 0) {
		set_from_loopfunc<DIRECTION,byte>(source, bitcount);
	}
}
template <bool ENDIAN>
template <std::size_t MAX_M>
void BitSequence<ENDIAN>::set_from(BitSequence& source, std::size_t bitcount) {
	// Adjusts bitcount to fit stream ranges
	bitcount = std::min(bitcount, std::min(bits_left(),source.bits_left()));

	// Choose copy direction to avoid overlapping-stream copy issues
	if (// *this and source are overlapping
		(p_index<source.p_index+source.bytes_covered_by_next_bits<FWD>(bitcount)
			&& source.p_index<p_index+bytes_covered_by_next_bits<FWD>(bitcount) )
		// and *this is ahead of source (s.t. source would override
		//  its end bytes when copying forward)
		&& (p_index>source.p_index || (p_index==source.p_index
			&& fwd_subindex<FWD>()>source.fwd_subindex<FWD>() ))
	) {
		//	  Sets bits by traversing bytes in reverse
		// skips fwd to last bit to be assigned
		fwd_skip_next<FWD>(bitcount-1);
		source.fwd_skip_next<FWD>(bitcount-1);
		// assigns bitcount bits in reverse
		//  (sets bit index in rev to that preceeding the first bit assigned)
		set_from_innerfunc<REV,MAX_M>(source,bitcount);
		// skips fwd to bit after last assigned bit
		fwd_skip_next<FWD>(bitcount+1);
		source.fwd_skip_next<FWD>(bitcount+1);
	}
	else set_from_innerfunc<FWD,MAX_M>(source,bitcount);
}
