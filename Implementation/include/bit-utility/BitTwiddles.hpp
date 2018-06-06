#ifndef BITTWIDDLES_H
#define BITTWIDDLES_H

#include <cstddef>
#include <climits>
#include <cstdint>

// NOTE - algorithms based on
//		https://graphics.stanford.edu/~seander/bithacks.html

template <typename MBYTE>
inline MBYTE lsb(MBYTE value) {
	return value & -value;
}
template <typename MBYTE>
MBYTE msb(MBYTE value) {
	for (std::size_t i = 1; i < sizeof(MBYTE)*CHAR_BIT; i <<= 1) {
		value |= value>>i;
	}
	return value ^ (value>>1);
}

/*
// NOTE - canNOT exempt char from mod unless table size < 256
//			(for char val = 0, val-1 == 255 -> canNOT fit in 128-width table)
//		- thus, 128-width table only wastes space compared to 64-width
static const uint8_t table_pow2m1mod131_0h[131] = {
	// 2^n mod131 indices in comments:
	//  0                          5
		       0,   1,  72,   2,  46,  73,  96,   3,  14,	//0
		 47,  56,  74,  18,  97, 118,   4,  43,  15,  35,	//10
		 48,  38,  57,  23,  75,  92,  19,  86,  98,  51,	//20
		119,  29,   5, 128,  44,  12,  16,  41,  36,  90,	//30
		 49, 126,  39, 124,  58,  60,  24, 105,  76,  62,	//40
		 93, 115,  20,  26,  87, 102,  99, 107,  52,  82,	//50
		120,  78,  30, 110,   6,  64, 129,  71,  45,  95,	//60
		 13,  55,  17, 117,  42,  34,  37,  22,  91,  85,	//70
		 50,  28, 127,  11,  40,  89, 125, 123,  59, 104,	//80
		 61, 114,  25, 101, 106,  81,  77, 109,  63,  70,	//90
		 94,  54, 116,  33,  21,  84,  27,  10,  88, 122,	//100
		103, 113, 100,  80, 108,  69,  53,  32,  83,   9,	//110
		121, 112,  79,  68,  31,   8, 111,  67,   7,  66,	//120
		 65, 130
	};
/*/
static const uint8_t table_pow2sub1mod67_0h[67] {
	// 2^n mod67 indices in comments:
	//  0                    5
		     0,	 1, 39,  2, 15, 40, 23,  3, 12,	//0
		16, 59, 41, 19, 24, 54,  4, 64, 13, 10,	//10
		17, 62, 60, 28, 42, 30, 20, 51, 25, 44,	//20
		55, 47,  5, 32, 65, 38, 14, 22, 11, 58,	//30
		18, 53, 63,  9, 61, 27, 29, 50, 43, 46,	//40
		31, 37, 21, 57, 52,  8, 26, 49, 45, 36,	//50
		56,  7, 48, 35,  6, 34, 33, 66         	//60
	};
//*/
template <typename MBYTE>
inline uint8_t bit_pos_0h(MBYTE value) {
	static_assert(
			sizeof(MBYTE)*CHAR_BIT < 67,
			"type width too large for table lookup"
		);
	//return table_pow2m1mod131_0h[sizeof(MBYTE)*CHAR_BIT > 8 ? (value-1)%131 : value-1];
	return table_pow2sub1mod67_0h[((MBYTE)(value-1)) % 67];
		// MBYTE cast required
		// otherwise uchar-1 -> char -> X
}


// Note - table size >= 128 allows for 'char'-type lookups w/o modulus operation
static const int8_t table_pow2mod131_0l[131] = {
	// 2^n mod131 indices in comments:
	//  0                          5
		 -1,   0,   1,  72,   2,  46,  73,  96,  3,  14,	//0
		 47,  56,  74,  18,  97, 118,   4,  43, 15,  35,	//10
		 48,  38,  57,  23,  75,  92,  19,  86, 98,  51,	//20
		119,  29,   5,  -3,  44,  12,  16,  41, 36,  90,	//30
		 49, 126,  39, 124,  58,  60,  24, 105, 76,  62,	//40
		 93, 115,  20,  26,  87, 102,  99, 107, 52,  82,	//50
		120,  78,  30, 110,   6,  64,  -2,  71, 45,  95,	//60
		 13,  55,  17, 117,  42,  34,  37,  22, 91,  85,	//70
		 50,  28, 127,  11,  40,  89, 125, 123, 59, 104,	//80
		 61, 114,  25, 101, 106,  81,  77, 109, 63,  70,	//90
		 94,  54, 116,  33,  21,  84,  27,  10, 88, 122,	//100
		103, 113, 100,  80, 108,  69,  53,  32, 83,   9,	//110
		121, 112,  79,  68,  31,   8, 111,  67,  7,  66,	//120
		 65
	};
template <typename MBYTE>
inline int8_t bit_pos_0l(MBYTE value) {
	static_assert(
			sizeof(MBYTE)*CHAR_BIT <= 128,
			"type width too large for table lookup"
		);
	return table_pow2mod131_0l[sizeof(MBYTE)*CHAR_BIT > 8 ? value%131 : value];
}

template <typename MBYTE>
inline int8_t bit_pos(MBYTE value) {
	return bit_pos_0l(value);
}


#endif
