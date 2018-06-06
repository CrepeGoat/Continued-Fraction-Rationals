#include <stdexcept>

template <bool ENDIAN>
WholeNumSeqSBSBase<ENDIAN> WholeNumSeqSBS1<ENDIAN>::sbsbase_copy_skip1bit() const {
	BitSequence<ENDIAN,false> bseq_copy(this->bseq);
	bseq_copy.skip_next();
	return WholeNumSeqSBSBase<ENDIAN>(bseq_copy);
}





template <bool ENDIAN>
bool WholeNumSeqSBS1<ENDIAN>::has_next() const {
	if (!this->bseq.has_next()) {
		return false;
	} else if (this->bseq.peek_next() == false) {
		return true;
	}

	return sbsbase_copy_skip1bit().skip_next();
}
template <bool ENDIAN>
bool WholeNumSeqSBS1<ENDIAN>::skip_next() {
	if (!this->bseq.has_next()) {
		return false;
	} else if (this->bseq.read_next() == false) {
		return true;
	}

	return WholeNumSeqSBSBase<ENDIAN>::skip_next();
}
template <bool ENDIAN>
bool WholeNumSeqSBS1<ENDIAN>::fits_next(const uintmax_t& value) const {
	if (value == 1) {
		return this->bseq.has_next();
	} else if (value == 0) {
		throw std::domain_error("WholeNumSeqSBS1::fits_next");
	}
	
	return sbsbase_copy_skip1bit().fits_next(value+1);
}

template <bool ENDIAN>
inline bool WholeNumSeqSBS1<ENDIAN>::peek_next(uintmax_t& value) const {
	return WholeNumSeqSBS1<ENDIAN>(*this).read_next(value);
}
template <bool ENDIAN>
bool WholeNumSeqSBS1<ENDIAN>::read_next(uintmax_t& value) {
	if (!this->bseq.has_next()) {
		return false;
	} else if (this->bseq.read_next() == false) {
		value = 1;
		return true;
	}

	if (!WholeNumSeqSBSBase<ENDIAN>::read_next(value)) {
		return false;
	}
	--value;
	return true;
}

template <bool ENDIAN>
bool WholeNumSeqSBS1<ENDIAN>::write_next(const uintmax_t& value) {
	if (value <= 0) {
		throw std::domain_error("WholeNumSeqSBS1::write_next");
	} else if (!this->bseq.has_next()) {
		this->bseq.skip_next(-1); // skips to end of BitSequence
		return false;
	}
	
	this->bseq.write_next(value > 1);
	return (value <= 1) || WholeNumSeqSBSBase<ENDIAN>::write_next(value+1);
}

template <bool ENDIAN>
inline WholeNumSeqSBS1<ENDIAN>& WholeNumSeqSBS1<ENDIAN>::operator>>(uintmax_t& value) {
	if (!read_next(value)) {
		throw std::range_error("WholeNumSeqSBS1::operator>>"
			" - incomplete entry stored");
	}
	return *this;
}
template <bool ENDIAN>
inline WholeNumSeqSBS1<ENDIAN>& WholeNumSeqSBS1<ENDIAN>::operator<<(const uintmax_t& value) {
	if (!write_next(value)) {
		throw std::range_error("WholeNumSeqSBS1::operator<<"
			" - insufficient space for argument");
	}
	return *this;
}
