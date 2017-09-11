#ifndef BITTWIDDLES_H
#define BITTWIDDLES_H

template <typename MBYTE>
MBYTE lsb(MBYTE value) {
	return value & -value;
}
template <typename MBYTE>
MBYTE msb(MBYTE value) {
	for (std::size_t i=1; i<CHAR_BIT*sizeof(MBYTE); i<<=1) {
		value |= value>>i;
	}
	return value^(value>>1);
}

template <typename MBYTE>
static std::size_t bit_pos(MBYTE value) {
	static const std::size_t pow2mod131_table[131] =
	{
		CHAR_BIT*sizeof(MBYTE),
	//  0					5
			0,	1,	72,	2,	46,	73,	96,	3,	14,	//0
		47,	56,	74,	18,	97,	118,4,	43,	15,	35,	//10
		48,	38,	57,	23,	75,	92,	19,	86,	98,	51,	//20
		119,29,	5,	128,44,	12,	16,	41,	36,	90,	//30
		49,	126,39,	124,58,	60,	24,	105,76,	62,	//40
		93,	115,20,	26,	87,	102,99,	107,52,	82,	//50
		120,78,	30,	110,6,	64,	129,71,	45,	95,	//60
		13,	55,	17,	117,42,	34,	37,	22,	91,	85,	//70
		50,	28,	127,11,	40,	89,	125,123,59,	104,//80
		61,	114,25,	101,106,81,	77,	109,63,	70,	//90
		94,	54,	116,33,	21,	84,	27,	10,	88,	122,//100
		103,113,100,80,	108,69,	53,	32,	83,	9,	//110
		121,112,79,	68,	31,	8,	111,67,	7,	66,	//120
		65
	};
	return pow2mod131_table[CHAR_BIT*sizeof(MBYTE) > 8 ? value%131 : value];
}

#endif
