#ifndef WHOLENUMSEQ_BASE_HPP
#define WHOLENUMSEQ_BASE_HPP

#include "BitSequence.hpp"

class WholeNumSeqBase {
protected:
	BitSequence<false> bseq;

public:
	explicit WholeNumSeqBase() = default;
	explicit WholeNumSeqBase(BitSequence<false> bits);
	void init(BitSequence<false> bits);
	BitSequence<false> read_bit_sequence();
};

#endif
