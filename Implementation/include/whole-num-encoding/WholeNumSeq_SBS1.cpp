#include "WholeNumSeq_SBS1.hpp"

#include <stdexcept>

WholeNumSeqSBSBase WholeNumSeqSBS1::sbsbase_copy_skip1bit() const {
	BitSequence<false> bseq_copy(this->bseq);
	bseq_copy.skip_next();
	return WholeNumSeqSBSBase(bseq_copy);
}





bool WholeNumSeqSBS1::has_next() const {
	if (!this->bseq.has_next()) {
		return false;
	} else if (this->bseq.peek_next() == false) {
		return true;
	}

	return sbsbase_copy_skip1bit().skip_next();
}
bool WholeNumSeqSBS1::skip_next() {
	if (!this->bseq.has_next()) {
		return false;
	} else if (this->bseq.read_next() == false) {
		return true;
	}

	return WholeNumSeqSBSBase::skip_next();
}
bool WholeNumSeqSBS1::fits_next(const uintmax_t& value) const {
	if (value == 1) {
		return this->bseq.has_next();
	} else if (value == 0) {
		throw std::domain_error("WholeNumSeqSBS1::fits_next");
	}
	
	return sbsbase_copy_skip1bit().fits_next(value+1);
}

bool WholeNumSeqSBS1::peek_next(uintmax_t& value) const {
	return WholeNumSeqSBS1(*this).read_next(value);
}
bool WholeNumSeqSBS1::read_next(uintmax_t& value) {
	if (!this->bseq.has_next()) {
		return false;
	} else if (this->bseq.read_next() == false) {
		value = 1;
		return true;
	}

	if (!WholeNumSeqSBSBase::read_next(value)) {
		return false;
	}
	--value;
	return true;
}

bool WholeNumSeqSBS1::write_next(const uintmax_t& value) {
	if (value <= 0) {
		throw std::domain_error("WholeNumSeqSBS1::write_next");
	} else if (!this->bseq.has_next()) {
		this->bseq.skip_next(-1); // skips to end of BitSequence
		return false;
	}
	
	this->bseq.write_next(value > 1);
	return (value <= 1) || WholeNumSeqSBSBase::write_next(value+1);
}

WholeNumSeqSBS1& WholeNumSeqSBS1::operator>>(uintmax_t& value) {
	if (!read_next(value)) {
		throw std::range_error("WholeNumSeqSBS1::operator>>"
			" - incomplete entry stored");
	}
	return *this;
}
WholeNumSeqSBS1& WholeNumSeqSBS1::operator<<(const uintmax_t& value) {
	if (!write_next(value)) {
		throw std::range_error("WholeNumSeqSBS1::operator<<"
			" - insufficient space for argument");
	}
	return *this;
}
