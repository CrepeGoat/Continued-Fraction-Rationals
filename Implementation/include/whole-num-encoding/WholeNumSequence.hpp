#ifndef WHOLENUMSEQUENCE_HPP
#define WHOLENUMSEQUENCE_HPP

#include "BitSequence.hpp"
#include "WholeNumSeq_SBS1.hpp"
#include "WholeNumSeq_SBS2.hpp"

//using namespace BitSequence_NS;

template <bool ENDIAN>
class WholeNumSequence
	:	private WholeNumSeqSBS1<ENDIAN>,
		private WholeNumSeqSBS2<ENDIAN>
{
private:
	struct {
		// regions: 0: [1] | 1: [2] | 2:[3,oo]
		unsigned a_nsub1_region : (CHAR_BIT-1);
		bool a_nsub2_is_lt3 : 1;
	} track_prev = {2,false};
	inline bool rho_leq_pt75() const;
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
