#ifndef WHOLENUMSEQ_SBS2_HPP
#define WHOLENUMSEQ_SBS2_HPP

#include "WholeNumSeq_SBSBase.hpp"
#include <cstdint>


class WholeNumSeqSBS2 : virtual protected WholeNumSeqSBSBase {
private:
	WholeNumSeqSBSBase sbsbase_copy_skip1bit() const;

public:
	using WholeNumSeqSBSBase::WholeNumSeqSBSBase;
	using WholeNumSeqSBSBase::init;
	using WholeNumSeqSBSBase::read_bit_sequence;

	bool has_next() const;
	bool skip_next();
	bool fits_next(const uintmax_t& value) const;

	// TODO make >>, << throw exceptions, read/write_next not check
	bool peek_next(uintmax_t& value) const;
	bool read_next(uintmax_t& value);
	bool write_next(const uintmax_t& value);

	WholeNumSeqSBS2& operator<<(const uintmax_t& u);
	WholeNumSeqSBS2& operator>>(uintmax_t& u);
};

#endif
