
template <bool ENDIAN>
WholeNumSeqBase<ENDIAN>::WholeNumSeqBase(BitSequence<ENDIAN,false> bits)
	: bseq(bits) {}

template <bool ENDIAN>
void WholeNumSeqBase<ENDIAN>::init(BitSequence<ENDIAN,false> bits) {
	bseq = bits;
}

template <bool ENDIAN>
BitSequence<ENDIAN,false> WholeNumSeqBase<ENDIAN>::read_bit_sequence() {
	return bseq;
}


