
#include "WholeNumSeq_Base.hpp"

WholeNumSeqBase::WholeNumSeqBase(BitSequence<false> bits)
	: bseq(bits) {}

void WholeNumSeqBase::init(BitSequence<false> bits) {
	bseq = bits;
}

BitSequence<false> WholeNumSeqBase::read_bit_sequence() {
	return bseq;
}


