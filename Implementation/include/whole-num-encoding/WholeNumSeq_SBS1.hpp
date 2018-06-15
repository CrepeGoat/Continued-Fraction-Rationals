#ifndef WHOLENUMSEQ_SBS1_HPP
#define WHOLENUMSEQ_SBS1_HPP

#include "WholeNumSeq_SBSBase.hpp"
#include <cstdint>


class WholeNumSeqSBS1 : virtual protected WholeNumSeqSBSBase {
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

	WholeNumSeqSBS1& operator<<(const uintmax_t& u);
	WholeNumSeqSBS1& operator>>(uintmax_t& u);
};

#endif
