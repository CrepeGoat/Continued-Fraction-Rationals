#include "TypeUtility.hpp"

#include <cstddef>	// std::size_t
#include <climits>	// CHAR_BIT
#include <limits>	// std::numeric_limits


template <typename MBYTE, typename INTEGER>
MBYTE shift_left(MBYTE bits, INTEGER n) {
	if (!std::numeric_limits<INTEGER>::is_signed) {
		return shift_left<MBYTE, typename to_signed<INTEGER>::type>(bits, n);
	}

	if (n >= 0) {
		return (n >= sizeof(MBYTE) * CHAR_BIT) ? 0 : bits << n;
	} else {
		return (-n >= sizeof(MBYTE) * CHAR_BIT) ? 0 : bits >> -n;
	}
}

template <typename MBYTE, typename INTEGER>
inline MBYTE shift_right(MBYTE bits, INTEGER n) {
	return shift_left(bits, -n);
}




template <typename MBYTE>
void apply_bits(MBYTE bits_from, MBYTE& bits_to, MBYTE mask) {
	bits_to ^= (bits_from^bits_to) & mask;
		// for all mask[i]==1, bits_to[i] -> bits_from[i]
		// for all mask[i]==0, bits_to[i] -> bits_to[i]
}




template <typename MBYTE, typename I1, typename I2>
MBYTE mask(I1 offset, I2 length) {
	return ~(~MBYTE(0) << length) << offset;
}






////////////////////////////////////////////////////////////////////////////////
// NOTE - below algorithms based on
//		https://graphics.stanford.edu/~seander/bithacks.html
////////////////////////////////////////////////////////////////////////////////

template <typename MBYTE>
MBYTE lsb(MBYTE value) {
	return value & -value;
}

template <typename MBYTE>
MBYTE msb(MBYTE value) {
	for (std::size_t i = 1; i < sizeof(MBYTE)*CHAR_BIT; i <<= 1) {
		value |= value>>i;
	}
	return value ^ (value>>1);
}




extern const uint8_t table_pow2sub1mod67_0h[67];

template <typename MBYTE>
uint8_t bit_pos_0h(MBYTE value) {
	static_assert(
			sizeof(MBYTE)*CHAR_BIT < 67,
			"type width too large for table lookup"
		);
	//return table_pow2m1mod131_0h[sizeof(MBYTE)*CHAR_BIT > 8 ? (value-1)%131 : value-1];
	return table_pow2sub1mod67_0h[((MBYTE)(value-1)) % 67];
		// MBYTE cast required
		// otherwise uchar-1 -> char   => NO BUENO
}




extern const int8_t table_pow2mod131_0l[131];

template <typename MBYTE>
int8_t bit_pos_0l(MBYTE value) {
	static_assert(
			sizeof(MBYTE)*CHAR_BIT <= 128,
			"type width too large for table lookup"
		);
	// Note - table size >= 128 allows for 'char'-type lookups w/o modulus operation
	return table_pow2mod131_0l[sizeof(MBYTE)*CHAR_BIT > 8 ? value%131 : value];
}




template <typename MBYTE>
int8_t bit_pos(MBYTE value) {
	return bit_pos_0l(value);
}
