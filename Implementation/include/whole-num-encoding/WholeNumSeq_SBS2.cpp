#include "WholeNumSeq_SBS2.hpp"

#include <stdexcept>

WholeNumSeqSBSBase WholeNumSeqSBS2::sbsbase_copy_skip1bit() const {
	BitSequence<false> bseq_copy(this->bseq);
	bseq_copy.skip_next();
	return WholeNumSeqSBSBase(bseq_copy);
}





bool WholeNumSeqSBS2::has_next() const {
	if (!this->bseq.has_next(2)) {
		return false;
	} else if (this->bseq.peek_next() == false) {
		return true;
	}
	
	return sbsbase_copy_skip1bit().skip_next();
}
bool WholeNumSeqSBS2::skip_next() {
	if (!this->bseq.has_next(2)) {
		return false;
	} else if (this->bseq.read_next() == false) {
		this->bseq.skip_next();
		return true;
	}

	return WholeNumSeqSBSBase::skip_next();
}
bool WholeNumSeqSBS2::fits_next(const uintmax_t& value) const {
	if (value == 0) {
		throw std::domain_error("WholeNumSeqSBS2::fits_next");
	} else if (value <= 2) {
		return this->bseq.has_next(2);
	}
	
	return sbsbase_copy_skip1bit().fits_next(value);
}

bool WholeNumSeqSBS2::peek_next(uintmax_t& value) const {
	return WholeNumSeqSBS2(*this).read_next(value);
}
bool WholeNumSeqSBS2::read_next(uintmax_t& value) {
	if (!this->bseq.has_next(2)) {
		return false;
	} else if (this->bseq.read_next() == false) {
		value = 1 + this->bseq.read_next();
		return true;
	} 

	return WholeNumSeqSBSBase::read_next(value);
}

bool WholeNumSeqSBS2::write_next(const uintmax_t& value) {
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
		return WholeNumSeqSBSBase::write_next(value);
	}
}

WholeNumSeqSBS2& WholeNumSeqSBS2::operator>>(uintmax_t& value) {
	if (!read_next(value)) {
		throw std::range_error("WholeNumSeqSBS2::operator>>"
			" - incomplete entry stored");
	}
	return *this;
}
WholeNumSeqSBS2& WholeNumSeqSBS2::operator<<(const uintmax_t& value) {
	if (!write_next(value)) {
		throw std::range_error("WholeNumSeqSBS2::operator<<"
			" - insufficient space for argument");
	}
	return *this;
}
