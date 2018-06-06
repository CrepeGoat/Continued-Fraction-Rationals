#include <stdexcept>

template <bool ENDIAN>
WholeNumSeqSBSBase<ENDIAN> WholeNumSeqSBS2<ENDIAN>::sbsbase_copy_skip1bit() const {
	BitSequence<ENDIAN,false> bseq_copy(this->bseq);
	bseq_copy.skip_next();
	return WholeNumSeqSBSBase<ENDIAN>(bseq_copy);
}





template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::has_next() const {
	if (!this->bseq.has_next(2)) {
		return false;
	} else if (this->bseq.peek_next() == false) {
		return true;
	}
	
	return sbsbase_copy_skip1bit().skip_next();
}
template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::skip_next() {
	if (!this->bseq.has_next(2)) {
		return false;
	} else if (this->bseq.read_next() == false) {
		this->bseq.skip_next();
		return true;
	}

	return WholeNumSeqSBSBase<ENDIAN>::skip_next();
}
template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::fits_next(const uintmax_t& value) const {
	if (value == 0) {
		throw std::domain_error("WholeNumSeqSBS2::fits_next");
	} else if (value <= 2) {
		return this->bseq.has_next(2);
	}
	
	return sbsbase_copy_skip1bit().fits_next(value);
}

template <bool ENDIAN>
inline bool WholeNumSeqSBS2<ENDIAN>::peek_next(uintmax_t& value) const {
	return WholeNumSeqSBS2<ENDIAN>(*this).read_next(value);
}
template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::read_next(uintmax_t& value) {
	if (!this->bseq.has_next(2)) {
		return false;
	} else if (this->bseq.read_next() == false) {
		value = 1 + this->bseq.read_next();
		return true;
	} 

	return WholeNumSeqSBSBase<ENDIAN>::read_next(value);
}

template <bool ENDIAN>
bool WholeNumSeqSBS2<ENDIAN>::write_next(const uintmax_t& value) {
	if (value <= 0) {
		throw std::domain_error("WholeNumSeqSBS2::write_next");
	} else if (!this->bseq.has_next(2)) {
		this->bseq.skip_next(-1); // skips to end of BitSequence
		return false;
	}

	if (value <= 2) {
		// Check for sufficient bit storage
		this->bseq.write_next(false);
		this->bseq.write_next(value == 2);
		return true;
	} else {
		this->bseq.write_next(true);
		return WholeNumSeqSBSBase<ENDIAN>::write_next(value);
	}
}

template <bool ENDIAN>
inline WholeNumSeqSBS2<ENDIAN>& WholeNumSeqSBS2<ENDIAN>::operator>>(uintmax_t& value) {
	if (!read_next(value)) {
		throw std::range_error("WholeNumSeqSBS2::operator>>"
			" - incomplete entry stored");
	}
	return *this;
}
template <bool ENDIAN>
inline WholeNumSeqSBS2<ENDIAN>& WholeNumSeqSBS2<ENDIAN>::operator<<(const uintmax_t& value) {
	if (!write_next(value)) {
		throw std::range_error("WholeNumSeqSBS2::operator<<"
			" - insufficient space for argument");
	}
	return *this;
}
