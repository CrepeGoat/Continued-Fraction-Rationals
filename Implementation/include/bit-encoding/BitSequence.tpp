#include <cstddef>
#include <cstdlib>
#include <climits>
#include <algorithm>
#include <cassert>

#include "BitTwiddles.hpp"

static const bool system_is_little_endian_runtime() {
	constexpr unsigned long long x(1);
	return (*((uint8_t*)&x) == 1);
}

template <typename MBYTE>
static void apply_bits(MBYTE bits_from, MBYTE& bits_to, MBYTE mask) {
	bits_to ^= (bits_from^bits_to) & mask;
		// for all mask[i]==1, bits_to[i] -> bits_from[i]
		// for all mask[i]==0, bits_to[i] -> bits_to[i]
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
std::size_t BitSequence<ENDIAN_LITTLE,BITS_L2M>::fwd_subindex() const {
	return BITS_L2M ? subindex : CHAR_BIT-1-subindex;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::set_fwd_subindex(std::size_t n) {
	subindex = BITS_L2M ? n : CHAR_BIT-1-n;
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <typename MBYTE>
MBYTE& BitSequence<ENDIAN_LITTLE,BITS_L2M>::fwd_mbyte() {
	return *(MBYTE*)(BYTES_ASCENDING() ? p_index : p_index - (sizeof(MBYTE)-1));
}
// Generic Methods for functionality independent of endianness
template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <typename MBYTE>
MBYTE BitSequence<ENDIAN_LITTLE,BITS_L2M>::fwd_shift(MBYTE value, std::ptrdiff_t n) {
	return std::abs(n) >= CHAR_BIT*sizeof(MBYTE) ? 0:
	(BITS_L2M == (n>=0)) ? value<<std::abs(n) : value>>std::abs(n);
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
std::ptrdiff_t BitSequence<ENDIAN_LITTLE,BITS_L2M>::bytes_left() const {
	return BYTES_ASCENDING() ? p_end-p_index : p_index-p_end;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::fwd_inc_p_index(std::size_t n) {
	if (BYTES_ASCENDING())
		p_index += n;
	else
		p_index -= n;
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
std::size_t BitSequence<ENDIAN_LITTLE,BITS_L2M>::bytes_covered_by_next_bits(
		std::size_t bitcount) const {
	// Also works v
	//return (bitcount+CHAR_BIT-1) / CHAR_BIT +
	//		(bitcount%CHAR_BIT + fwd_subindex() > CHAR_BIT);
	return (bitcount == 0) ? 0 :
		(bitcount + fwd_subindex() + CHAR_BIT-1) / CHAR_BIT;
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::fwd_skip_next(std::size_t bitcount) {
	fwd_inc_p_index((fwd_subindex()+bitcount)/CHAR_BIT);
	set_fwd_subindex((fwd_subindex()+bitcount)%CHAR_BIT);
}








// Constructors
template <bool ENDIAN_LITTLE, bool BITS_L2M>
BitSequence<ENDIAN_LITTLE,BITS_L2M>::BitSequence() : p_index(nullptr), p_end(nullptr) {
	assert(sizeof(unsigned long long) == sizeof(unsigned char)
			|| ENDIAN_LITTLE == system_is_little_endian_runtime()
		);
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
BitSequence<ENDIAN_LITTLE,BITS_L2M>::BitSequence(uint8_t* begin, uint8_t* end)
	:	subindex(BITS_L2M ? 0 : CHAR_BIT-1),
		p_index(BYTES_ASCENDING() ? begin : end-1),
		p_end(BYTES_ASCENDING() ? std::max(begin,end) : std::min(begin,end)-1)
{
	assert(sizeof(unsigned long long) == sizeof(uint8_t)
			|| ENDIAN_LITTLE == system_is_little_endian_runtime()
		);
}
//template <bool ENDIAN_LITTLE, bool BITS_L2M>
//BitSequence<ENDIAN_LITTLE,BITS_L2M>::BitSequence(const BitSequence& rhs) :
//		subindex(rhs.subindex),
//		p_index(rhs.p_index),
//		p_end(rhs.p_end) {}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::init(uint8_t* begin, uint8_t* end) {
	set_fwd_subindex(0);
	p_index = BYTES_ASCENDING() ? begin : end-1;
	p_end = BYTES_ASCENDING() ? std::max(begin,end) : std::min(begin,end)-1;
}


template <bool ENDIAN_LITTLE, bool BITS_L2M>
BitSequence<ENDIAN_LITTLE,BITS_L2M>::operator bool() const {
	return has_next();
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
std::size_t BitSequence<ENDIAN_LITTLE,BITS_L2M>::bits_left() const {
	return CHAR_BIT*sizeof(uint8_t)*bytes_left() - fwd_subindex();
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool BitSequence<ENDIAN_LITTLE,BITS_L2M>::has_next() const {
	return bytes_left() > 0;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool BitSequence<ENDIAN_LITTLE,BITS_L2M>::has_next(std::size_t bitcount) const 
{
	return bitcount <= bits_left();
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::skip_next(std::size_t bitcount) {
	fwd_skip_next(std::min(bits_left(), bitcount));
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool BitSequence<ENDIAN_LITTLE,BITS_L2M>::peek_next() const {
	if (has_next()) {
		return (((uint8_t(1)<<subindex) & *p_index) != 0);
	}
	return bool();
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool BitSequence<ENDIAN_LITTLE,BITS_L2M>::read_next() {
	bool bit;
	if (has_next()) {
		bit = (((uint8_t(1)<<subindex) & *p_index) != 0);
		fwd_skip_next(1);
	}
	return bit;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::write_next(bool bit) {
	if (has_next()) {
		apply_bits<uint8_t>(uint8_t(bit)<<subindex, *p_index, 1<<subindex);
		fwd_skip_next(1);
	}
}


template <bool ENDIAN_LITTLE, bool BITS_L2M>
inline BitSequence<ENDIAN_LITTLE,BITS_L2M>& BitSequence<ENDIAN_LITTLE,BITS_L2M>
		::operator>>(bool& bit)
{
	bit = read_next();
	return *this;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
inline BitSequence<ENDIAN_LITTLE,BITS_L2M>& BitSequence<ENDIAN_LITTLE,BITS_L2M>
		::operator<<(bool bit)
{
	write_next(bit);
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
template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <typename MBYTE>
std::size_t BitSequence<ENDIAN_LITTLE,BITS_L2M>::fwd_subindex_first_1bit(
		MBYTE value)
{
	return BITS_L2M
			? bit_pos_0h(lsb(value))
			: CHAR_BIT*sizeof(MBYTE)-1 - bit_pos_0l(msb(value));
}


template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <typename MBYTE>
bool BitSequence<ENDIAN_LITTLE,BITS_L2M>::read_streak_loopfunc(
		bool bit, std::size_t& bitcount) {
	// Saves position of first non-"bit" bit value
	const std::size_t len_advance = std::min(bitcount,
			fwd_subindex_first_1bit<MBYTE>(		// position of first_1bit of
				(-MBYTE(bit) ^ fwd_mbyte<MBYTE>()) &	// (non-)inverted MBYTE
				fwd_shift<MBYTE>(-1, fwd_subindex())	// w/ high-pass bit mask
			) - fwd_subindex()			// minus current bit position
		);
	bitcount -= len_advance;
	fwd_skip_next(len_advance);
	return (fwd_subindex() == 0 && len_advance > CHAR_BIT*(sizeof(MBYTE)-1)
			// ^ check if search hit end of mbyte
			&& bitcount > 0 // and if there are more bits to check
		);
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <std::size_t MAX_M>
std::size_t BitSequence<ENDIAN_LITTLE,BITS_L2M>::read_streak(
		bool bit, std::size_t max_bitcount) {
	max_bitcount = std::min(max_bitcount, bits_left());
	std::size_t bitcount = max_bitcount;

	#define read_streak_innermacro(T,SIZEOF_T_prev) \
		if (sizeof(T) <= std::min(MAX_M, SIZEOF_T_prev-1)) { \
			while (bytes_covered_by_next_bits(bitcount) >= sizeof(T)) { \
				if (!read_streak_loopfunc<T>(bit, bitcount)) { \
					goto RETURN; \
				} \
			} \
		}
	read_streak_innermacro(unsigned long long,std::size_t(-1))
	read_streak_innermacro(unsigned long,sizeof(unsigned long long))
	read_streak_innermacro(unsigned int,sizeof(unsigned long))
	read_streak_innermacro(unsigned short,sizeof(unsigned int))
	read_streak_innermacro(uint8_t,sizeof(unsigned short))
	RETURN:
	#undef read_streak_innermacro

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
template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <typename MBYTE>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::write_streak_loopfunc(
		bool bit, std::size_t& bitcount) {
	//if (bitcount<=0) return false;
	const std::size_t len_advance = std::min(bitcount,
			CHAR_BIT*sizeof(MBYTE)-fwd_subindex()
		);
	apply_bits<MBYTE>(-bit, fwd_mbyte<MBYTE>(), fwd_shift<MBYTE>(
			~fwd_shift<MBYTE>(-1, len_advance), fwd_subindex()
		));
	bitcount -= len_advance;
	fwd_skip_next(len_advance);
	//return true;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <std::size_t MAX_M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::write_streak(
		bool bit, std::size_t bitcount) {
	// Adjusts bitcount to fit stream range
	bitcount = std::min(bitcount, bits_left());
	// Passes bits in as large of byte blocks as possible
	#define write_streak_innermacro(T,SIZEOF_T_prev) \
		if (sizeof(T) <= MAX_M && sizeof(T) < SIZEOF_T_prev) { \
			while (sizeof(T) <= bytes_covered_by_next_bits(bitcount)) { \
				write_streak_loopfunc<T>(bit, bitcount); \
			} \
		}
	write_streak_innermacro(unsigned long long,std::size_t(-1))
	write_streak_innermacro(unsigned long,sizeof(unsigned long long))
	write_streak_innermacro(unsigned int,sizeof(unsigned long))
	write_streak_innermacro(unsigned short,sizeof(unsigned int))
	#undef write_streak_innermacro
	// Passes remaining bits by single byte
	while (bitcount > 0) {
		write_streak_loopfunc<uint8_t>(bit, bitcount);
	}
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <typename MBYTE>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::write_from_int(
		MBYTE source, std::size_t bitcount) {
	bitcount = std::min(bitcount, CHAR_BIT*sizeof(MBYTE));
	BitSequence<ENDIAN_LITTLE,BITS_L2M> bseq2(
			(uint8_t*)&source, (uint8_t*)(&source)+sizeof(MBYTE)
		);
	if (!BITS_L2M) {
		bseq2.skip_next(CHAR_BIT*sizeof(MBYTE) - bitcount);
	}
	write_from(bseq2, bitcount);
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <typename MBYTE>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::read_to_int(
		MBYTE& dest, std::size_t bitcount) {
	bitcount = std::min(bitcount, CHAR_BIT*sizeof(MBYTE));
	BitSequence<ENDIAN_LITTLE,BITS_L2M> bseq2(
			(uint8_t*)&dest, (uint8_t*)(&dest)+sizeof(MBYTE)
		);
	if (!BITS_L2M) {
		bseq2.skip_next(CHAR_BIT*sizeof(MBYTE) - bitcount);
	}
	bseq2.write_from(*this, bitcount);
}

/* FUNCTION		 write_from
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
template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <typename MBYTE>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::write_from_loopfunc(
		BitSequence& source,
		std::size_t& bitcount
	) {
	const std::size_t len_advance = std::min<std::size_t>(
			bitcount, CHAR_BIT*sizeof(MBYTE) - std::max(
				fwd_subindex(),
				source.fwd_subindex()
			)
		);
	// Applies bits in BLOCK of bytes
	apply_bits<MBYTE>(
			// Source of BLOCK copy
			fwd_shift<MBYTE>(
				source.fwd_mbyte<MBYTE>(),
				fwd_subindex()-source.fwd_subindex()
			),
			// Target of BLOCK copy
			fwd_mbyte<MBYTE>(),
			// Mask for copy
			fwd_shift<MBYTE>(
				~fwd_shift<MBYTE>(-1,len_advance),
				fwd_subindex()
			)
		);

	bitcount -= len_advance;
	fwd_skip_next(len_advance);
	source.fwd_skip_next(len_advance);
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <std::size_t MAX_M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::write_from_innerfunc(
		BitSequence& source, std::size_t bitcount) {
	// Passes bits in as many m-byte blocks as possible
	//  without exceeding streams' byte-assignment ranges
	// (though assigning to external bytes w/ mask would not alter data,
	//  it could result in race issues if simultaneously assigning
	//  in another program/stream)
	#define write_from_innermacro(T,SIZEOF_T_prev) \
		if (sizeof(T) <= MAX_M && sizeof(T) < SIZEOF_T_prev) { \
			while (sizeof(T) <= std::min( \
				bytes_covered_by_next_bits(bitcount), \
				source.bytes_covered_by_next_bits(bitcount) \
			)) { \
				write_from_loopfunc<T>(source, bitcount); \
			} \
		}
	write_from_innermacro(unsigned long long,std::size_t(-1))
	write_from_innermacro(unsigned long,sizeof(unsigned long long))
	write_from_innermacro(unsigned int,sizeof(unsigned long))
	write_from_innermacro(unsigned short,sizeof(unsigned int))
	#undef write_from_innermacro
	// Passes remaining bits in byte blocks
	while (bitcount > 0) {
		write_from_loopfunc<uint8_t>(source, bitcount);
	}
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
template <std::size_t MAX_M>
void BitSequence<ENDIAN_LITTLE,BITS_L2M>::write_from(
		BitSequence& source, std::size_t bitcount) {
	// Adjusts bitcount to fit stream ranges
	bitcount = std::min(bitcount, std::min(bits_left(),source.bits_left()));
	// TODO revise conditions to work for BYTES_ASCENDING() == (true / false)
	// Choose copy direction to avoid overlapping-stream copy issues
	const std::ptrdiff_t diff = (*this) - source;
	if (// source would override its end bytes when copying forward; i.e.,
			diff > 0				// *this is ahead of source
			&& diff < bitcount		// and *this overlaps source
		) {
		//	  Sets bits by traversing bytes in reverse
		// skips fwd to last bit to be assigned
		fwd_skip_next(bitcount-1);
		source.fwd_skip_next(bitcount-1);
		// assigns bitcount bits in reverse
		//  (sets bit index in rev to that preceeding the first bit assigned)
		// NOTE - reinterpret_cast canNOT work if p_end is used in any way
		typedef BitSequence<ENDIAN_LITTLE,!BITS_L2M> BitSequence_REV;
		reinterpret_cast<BitSequence_REV*>(this)->write_from_innerfunc<MAX_M>(
				*reinterpret_cast<BitSequence_REV*>(&source), bitcount);
		// skips fwd to bit after forward-most assigned bit
		fwd_skip_next(bitcount+1);
		source.fwd_skip_next(bitcount+1);
	}
	else if (diff != 0)
		write_from_innerfunc<MAX_M>(source,bitcount);
}


template <bool ENDIAN_LITTLE, bool BITS_L2M>
std::ptrdiff_t operator-(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	) {
	return CHAR_BIT * (BitSequence<ENDIAN_LITTLE,BITS_L2M>::BYTES_ASCENDING()
			? bseq1.p_index-bseq2.p_index
			: bseq2.p_index-bseq1.p_index
		) + bseq1.fwd_subindex()-bseq2.fwd_subindex();
}

template <bool ENDIAN_LITTLE, bool BITS_L2M>
inline bool operator<(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	) {
	return bseq2 - bseq1 > 0;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
inline bool operator<=(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	) {
	return !(bseq1 > bseq2);
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
inline bool operator>(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	) {
	return bseq2 < bseq1;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
inline bool operator>=(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	) {
	return !(bseq1 < bseq2);
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool operator==(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	) {
	return bseq1.p_index == bseq2.p_index &&
			bseq1.subindex == bseq2.subindex;
}
template <bool ENDIAN_LITTLE, bool BITS_L2M>
inline bool operator!=(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	) {
	!(bseq1 == bseq2);
}
