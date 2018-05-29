#ifndef WHOLENUMSEQUENCE_HPP
#define WHOLENUMSEQUENCE_HPP

#include "BitSequence.hpp"
#include "WholeNumSeq_SBS1.hpp"
#include "WholeNumSeq_SBS2.hpp"

template <bool ENDIAN>
class WholeNumSequence
	:	private WholeNumSeqSBS1<ENDIAN>,
		private WholeNumSeqSBS2<ENDIAN>
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
	inline bool rho_lt_3div4() const;
	inline void update_rho(uintmax_t n);
	
public:
	using WholeNumSeqBase<ENDIAN>::WholeNumSeqBase;
	using WholeNumSeqBase<ENDIAN>::read_bit_sequence;
	
	inline void init(BitSequence<ENDIAN,false> bits);

	inline bool has_next() const;
	inline void skip_next();
	inline bool fits_next(const uintmax_t& value) const;

	// TODO make >>, << throw exceptions, read/write_next not check
	inline bool peek_next(uintmax_t& value) const;
	inline bool read_next(uintmax_t& value);
	inline bool write_next(uintmax_t value);

	inline WholeNumSequence& operator<<(const uintmax_t& u);
	inline WholeNumSequence& operator>>(uintmax_t& u);
};

#include "WholeNumSequence.tpp"

#endif
