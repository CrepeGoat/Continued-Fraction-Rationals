#ifndef BITSEQUENCE_H
#define BITSEQUENCE_H

#include <cstddef>

typedef unsigned char byte;

/* CLASS: BitSequence
 *
 * Stores bits in an array of bytes. Bits are ordered:
 *		- by byte in ascending address order
 *		- within each byte depending on system endianess
 *			- little endian - from LSB to MSB
 *			- big endian - from MSB to LSB
 *		This byte ordering scheme allows for assignment/retrieval of bits by
 *		DWORD or QWORD, such that the D/QWORD bit order from LSB to MSB is
 *		maintained when bytes are grouped into d/qwords at various offset
 *		positions.
 *
 * Template Parameters
 *  - ENDIAN - the endianness of the system
 *		   - adjusts internal storage behavior
 *
 * - MAX_M - the maximum byte-length to be assigned at once
 * (NOTE: to get a predictable encoding scheme between systems of
 *		differing endiannesses, use the same ENDIAN value and set MAX_M=1)
 *
 * - (internal) DIRECTION
 *		- whether the bit/byte pointer increments or decrements
 *		- chosen automatically to avoid overwrite errors when copying bits
 *			between overlapping bit sequences
 *		- (chosen arbitrarilly when sequences do not overlap)
 *
 * TODO:
 * - change char pointer into generic char iterator
 */
template <bool ENDIAN_LITTLE, bool BITS_L2M>
class BitSequence;

//*
// COMPARISON OPERATORS
template <bool ENDIAN_LITTLE, bool BITS_L2M>
std::ptrdiff_t operator-(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	);

template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool operator<(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	);
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool operator<=(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	);
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool operator>(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	);
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool operator>=(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	);
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool operator==(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	);
template <bool ENDIAN_LITTLE, bool BITS_L2M>
bool operator!=(
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
		const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
	);
//*/

template <bool ENDIAN_LITTLE, bool BITS_L2M>
class BitSequence {
private:
	friend BitSequence<ENDIAN_LITTLE,!BITS_L2M>;
	friend bool operator==<ENDIAN_LITTLE,BITS_L2M>(
			const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
			const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
		);
	friend std::ptrdiff_t operator-<ENDIAN_LITTLE,BITS_L2M>(
			const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq1,
			const BitSequence<ENDIAN_LITTLE,BITS_L2M>& bseq2
		);

	static inline bool BYTES_ASCENDING() {
		return ENDIAN_LITTLE == BITS_L2M;
	}

	byte subindex;
	byte* p_index;
	byte* p_end;

	//template <typename MBYTE>
	//static inline void apply_bits(MBYTE bits_from, MBYTE& bits_to, MBYTE mask);

	/*
	 * Generic Methods that provide an abstraction independent of
	 *	endianness and assignment direction
	 */
	// Returns the stream-ordered bit position of the next bit
	inline std::size_t fwd_subindex() const;
	// Moves the bit pointer to the stream-ordered 'n'th bit position
	inline void set_fwd_subindex(std::size_t n);
	// Returns the MBYTE containing the next bit and the maximum number of
	//	following bits (i.e. the next bit will be in the "first" byte of the
	//	MBYTE, but not necessarily in the "first" bit position)
	template <typename MBYTE>
	inline MBYTE& fwd_mbyte();
	// Returns the MBYTE resulting from bit-shifting 'value' forward 'n'
	//	positions
	template <typename MBYTE>
	static inline MBYTE fwd_shift(MBYTE value, std::ptrdiff_t n);

	// Returns the number of bytes remaining in the stream
	inline std::ptrdiff_t bytes_left() const;
	// Increments the byte pointer "forward" 'n' positions
	inline void fwd_inc_p_index(std::size_t n);

	// Returns the number of bytes overlapped with the next 'bitcount' bits
	//	in the stream
	inline std::size_t bytes_covered_by_next_bits(std::size_t bitcount) const;

	// Skips the next 'bitcount' bits in the stream
	inline void fwd_skip_next(std::size_t bitcount);


public:
	// TODO allow for initial offset
	explicit BitSequence();
	explicit BitSequence(byte* begin, byte* end);
	//BitSequence(const BitSequence<ENDIAN>& rhs);
	void init(byte* begin, byte* end);

	operator bool() const;
	inline std::size_t bits_left() const;
	bool has_next() const;
	bool has_next(std::size_t bitcount) const;
	void skip_next(std::size_t bitcount=1);

	// TODO make >>, << throw exceptions, read/write_next not check
	bool peek_next() const;
	bool read_next();
	void write_next(bool bit);

	BitSequence& operator>>(bool& bit);
	BitSequence& operator<<(bool bit);


	//BitSequence& get(unsigned long long &target, std::size_t bitcount);
	//BitSequence& set(const unsigned long long &source, std::size_t bitcount);

	// Advances the stream 'max_bitcount' bits, until EOS, or up to the next
	//	bit that is not equal to the input, 'bit'. Returns the number of
	//	bits advanced.
	template <std::size_t MAX_M=std::size_t(-1)>
	std::size_t read_streak(bool bit, std::size_t max_bitcount=std::size_t(-1));

	// Writes the input, 'bit', over the next 'bitcount' bits. Advances the
	//	stream to the following bit.
	template <std::size_t MAX_M=std::size_t(-1)>
	void write_streak(bool bit, std::size_t bitcount);

	template <typename MBYTE>
	void write_from_int(MBYTE source, std::size_t bitcount);
	template <typename MBYTE>
	void read_to_int(MBYTE& dest, std::size_t bitcount);

	// Writes over the next 'bitcount' bits with the next 'bitcount' bits
	//	from 'source'. Advances both streams to the respective following bits.
	template <std::size_t MAX_M=std::size_t(-1)>
	void write_from(BitSequence& source, std::size_t bitcount);

private:
	template <typename MBYTE>
	static inline std::size_t fwd_subindex_first_1bit(MBYTE value);
	// Function that is looped inside the 'get' method
	template <typename MBYTE>
	bool read_streak_loopfunc(bool bit, std::size_t& bitcount);
	// Function that is looped inside the 'set' method
	template <typename MBYTE>
	void write_streak_loopfunc(bool bit, std::size_t& bitcount);
	// Function that is looped inside the 'write_from' method
	template <typename MBYTE>
	void write_from_loopfunc(BitSequence& source, std::size_t& bitcount);
	// Function that is nested inside the 'write_from' method
	template <std::size_t MAX_M>
	void write_from_innerfunc(BitSequence& source, std::size_t bitcount);
};

#include "BitSequence.tpp"


#endif
