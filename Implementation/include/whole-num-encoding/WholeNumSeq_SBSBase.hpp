#ifndef WHOLENUMSEQ_SBSBASE_HPP
#define WHOLENUMSEQ_SBSBASE_HPP

#include "WholeNumSeq_Base.hpp"
#include <cstdint>

class WholeNumSeqSBSBase : public WholeNumSeqBase {
public:
	using WholeNumSeqBase::WholeNumSeqBase;

	bool has_next() const;
	bool skip_next();
	bool fits_next(const uintmax_t& value) const;

	// TODO make >>, << throw exceptions, read/write_next not check
	bool peek_next(uintmax_t& value) const;
	bool read_next(uintmax_t& value);
	bool write_next(const uintmax_t& value);

	WholeNumSeqSBSBase& operator<<(const uintmax_t& u);
	WholeNumSeqSBSBase& operator>>(uintmax_t& u);
};

#endif
