#ifndef WHOLENUMSEQUENCE_H
#define WHOLENUMSEQUENCE_H

#include "BitSequence.h"
#include <cstddef>

//using namespace BitSequence_NS;

template <bool ENDIAN>
class WholeNumSequence {
private:
	BitSequence<ENDIAN> bseq;

public:
	typedef unsigned long long wnum_t;
	static std::size_t encoding_bitlength(wnum_t n);

	explicit WholeNumSequence();
	explicit WholeNumSequence(BitSequence<ENDIAN> bits);
	void init(BitSequence<ENDIAN> bits);
	BitSequence<ENDIAN> get_bit_sequence();

	bool has_next() const;
	void skip_next();
	bool fits_next(const wnum_t& value) const;

	// TODO make >>, << throw exceptions, get/set_next not check
	bool peek_next(wnum_t& value) const;
	bool get_next(wnum_t& value);
	bool set_next(wnum_t value);

	WholeNumSequence& operator<<(const wnum_t& u);
	WholeNumSequence& operator>>(wnum_t& u);
};

#include "WholeNumSequence_cpp.h"

#endif
