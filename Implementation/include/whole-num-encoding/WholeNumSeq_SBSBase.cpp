#include "WholeNumSeq_SBSBase.hpp"

#include <stdexcept>

bool WholeNumSeqSBSBase::has_next() const {
	BitSequence<false> bseq_copy(this->bseq);
	const std::size_t len_1s_prefix = bseq_copy.read_streak(true);
	// Preliminary check (ensures skip_next() will not fail)
	if (!bseq_copy.has_next(len_1s_prefix+2)) {
		return false;
	}
	// Gets second-most-sig-bit
	bseq_copy.skip_next();
	// Decisive check (informed by second-most-sig-bit)
	return !bseq_copy.peek_next() || bseq_copy.has_next(len_1s_prefix + 2);
}
bool WholeNumSeqSBSBase::skip_next() {
	const std::size_t len_1s_prefix = this->bseq.read_streak(true);
	// Preliminary check (ensures skip_next() will not fail)
	if (!this->bseq.has_next(len_1s_prefix+2)) {
		this->bseq.skip_next(-1);
		return false;
	}
	// Performs full skip (cannot fail with insufficient space)
	this->bseq.skip_next(len_1s_prefix + 2 + this->bseq.peek_next());
	return true;
}
bool WholeNumSeqSBSBase::fits_next(const uintmax_t& value) const {
	// Handle null cases
	if (value <= 2) {
		throw std::domain_error("WholeNumSeqSBSBase::write_next");
	}
	// Get significant variables
	std::size_t pos_msb = msb(value);
	const bool smsb = value & (pos_msb>>1);
	pos_msb = bit_pos_0h(pos_msb);
	// Check for sufficient bit storage
	return this->bseq.has_next(2*pos_msb - !smsb);
}

inline bool WholeNumSeqSBSBase::peek_next(uintmax_t& value) const {
	return WholeNumSeqSBSBase(*this).read_next(value);
}
bool WholeNumSeqSBSBase::read_next(uintmax_t& value) {
	// Get significant variables
	const std::size_t len_1s_prefix = this->bseq.read_streak(true);
	// Make preliminary check
	if (!this->bseq.has_next(len_1s_prefix+2)) {
		this->bseq.skip_next(-1);
		return false;
	}
	this->bseq.skip_next(); // guaranteed 0-bit after end of 1's streak
	// Get other significant variables
	const bool smsb = !this->bseq.read_next();
	const std::size_t pos_smsb = len_1s_prefix + !smsb;

	if (!this->bseq.has_next(pos_smsb)) {
		this->bseq.skip_next(-1);
		return false;
	}
	// Write bits to integer
	value = 0;
	this->bseq.read_to_int(value, pos_smsb);
	value += (uintmax_t(2+smsb) << pos_smsb);
	
	return true;
}

bool WholeNumSeqSBSBase::write_next(const uintmax_t& value) {
	// Handle null cases
	if (value <= 2) {
		throw std::domain_error("WholeNumSeqSBSBase::write_next");
	}

	// Get significant variables
	std::size_t pos_msb = msb(value);
	const bool smsb = value & (pos_msb>>1);
	pos_msb = bit_pos_0h(pos_msb);
	
	// Check for sufficient bit storage
	if (!this->bseq.has_next(2*pos_msb + smsb - 1)) {
		this->bseq.skip_next(-1); // skips to end of BitSequence
		return false;
	}
	
	// Set bits
	this->bseq.write_streak(true, pos_msb + smsb - 2);
	this->bseq << false << !smsb;
	this->bseq.write_from_int(value, pos_msb-1);

	return true;
}

WholeNumSeqSBSBase& WholeNumSeqSBSBase::operator>>(uintmax_t& value) {
	if (!this->read_next(value)) {
		throw std::range_error("WholeNumSeqSBSBase::operator>>"
			" - incomplete entry stored");
	}
	return *this;
}
WholeNumSeqSBSBase& WholeNumSeqSBSBase::operator<<(const uintmax_t& value) {
	if (!this->write_next(value)) {
		throw std::range_error("WholeNumSeqSBSBase::operator<<"
			" - insufficient space for argument");
	}
	return *this;
}
