#ifndef WHOLENUMSEQ_SBS1_HPP
#define WHOLENUMSEQ_SBS1_HPP

#include "WholeNumSeq_Base.hpp"
#include <cstdint>

//using namespace BitSequence_NS;

template <bool ENDIAN>
class WholeNumSeqSBS1 : virtual public WholeNumSeqBase<ENDIAN> {
public:
	using WholeNumSeqBase<ENDIAN>::WholeNumSeqBase;

	static std::size_t encoding_bitlength(uintmax_t n);

	bool has_next() const;
	void skip_next();
	bool fits_next(const uintmax_t& value) const;

	// TODO make >>, << throw exceptions, read/write_next not check
	bool peek_next(uintmax_t& value) const;
	bool read_next(uintmax_t& value);
	bool write_next(uintmax_t value);

	WholeNumSeqSBS1& operator<<(const uintmax_t& u);
	WholeNumSeqSBS1& operator>>(uintmax_t& u);
};

#include "WholeNumSeq_SBS1.tpp"

#endif
