#ifndef WHOLENUMSEQ_BASE_HPP
#define WHOLENUMSEQ_BASE_HPP

#include "BitSequence.hpp"

template <bool ENDIAN>
class WholeNumSeqBase {
protected:
	BitSequence<ENDIAN,false> bseq;

public:
	explicit WholeNumSeqBase() = default;
	explicit WholeNumSeqBase(BitSequence<ENDIAN,false> bits);
	void init(BitSequence<ENDIAN,false> bits);
	BitSequence<ENDIAN,false> read_bit_sequence();
};

#include "WholeNumSeq_Base.tpp"

#endif
