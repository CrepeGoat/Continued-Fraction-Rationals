#ifndef WHOLENUMSEQUENCE_HPP
#define WHOLENUMSEQUENCE_HPP

#include "BitSequence.hpp"
#include "WholeNumSeq_SBS1.hpp"
#include "WholeNumSeq_SBS2.hpp"


class WholeNumSequence
	:	private WholeNumSeqSBS1,
		private WholeNumSeqSBS2
{
private:
	enum RhoRegion : int8_t {
		EQ_0,
		LEQ_1DIV3,
		IN_1DIV3_3DIV4,
		GEQ_3DIV4,
		EQ_1
	};
	RhoRegion rho_region = EQ_0;
	bool rho_lt_3div4() const;
	void update_rho(uintmax_t n);
	
public:
	using WholeNumSeqSBSBase::WholeNumSeqSBSBase;
	using WholeNumSeqSBSBase::read_bit_sequence;
	
	void init(BitSequence<false> bits);

	bool has_next() const;
	void skip_next();
	bool fits_next(const uintmax_t& value) const;

	// TODO make >>, << throw exceptions, read/write_next not check
	bool peek_next(uintmax_t& value) const;
	bool read_next(uintmax_t& value);
	bool write_next(uintmax_t value);

	WholeNumSequence& operator<<(const uintmax_t& u);
	WholeNumSequence& operator>>(uintmax_t& u);
};

#endif
