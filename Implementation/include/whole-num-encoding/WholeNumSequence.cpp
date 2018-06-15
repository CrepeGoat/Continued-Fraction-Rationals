#include "WholeNumSequence.hpp"
extern template class BitSequence<false>;


inline bool WholeNumSequence::rho_lt_3div4() const {
	return rho_region < GEQ_3DIV4;
}
void WholeNumSequence::update_rho(uintmax_t a_n) {
	//		rho_n = (a_n + rho_(n-1)) ^ -1

	// (a_n <= 0) -> not permitted
	
	if (a_n == 1) {
		if (rho_region == EQ_0)
			rho_region = EQ_1;
		else if (rho_region == LEQ_1DIV3)
			rho_region = GEQ_3DIV4;
		else
			rho_region = IN_1DIV3_3DIV4;
	}

	else if (a_n == 2) {
		if (rho_region < EQ_1)
			rho_region = IN_1DIV3_3DIV4;
		else
			rho_region = LEQ_1DIV3;
	}
	
	else { // if (a_n > 2)
		rho_region = LEQ_1DIV3;
	}
}


void WholeNumSequence::init(BitSequence<false> bits) {
	WholeNumSeqBase::init(bits);
	rho_region = EQ_0;
}



bool WholeNumSequence::has_next() const {
	return (rho_lt_3div4()
		? WholeNumSeqSBS1::has_next()
		: WholeNumSeqSBS2::has_next());
}
void WholeNumSequence::skip_next() {
	uintmax_t value;
	if (rho_lt_3div4()
			? WholeNumSeqSBS1::read_next(value)
			: WholeNumSeqSBS2::read_next(value)
		)
		update_rho(value);
}
bool WholeNumSequence::fits_next(const uintmax_t& value) const {
	return (rho_lt_3div4()
		? WholeNumSeqSBS1::fits_next(value)
		: WholeNumSeqSBS2::fits_next(value));
}

bool WholeNumSequence::peek_next(uintmax_t& value) const {
	return (rho_lt_3div4()
		? WholeNumSeqSBS1::peek_next(value)
		: WholeNumSeqSBS2::peek_next(value));
}
bool WholeNumSequence::read_next(uintmax_t& value) {
	if (!(rho_lt_3div4()
			? WholeNumSeqSBS1::read_next(value)
			: WholeNumSeqSBS2::read_next(value)))
		return false;

	update_rho(value);
	return true;
}

bool WholeNumSequence::write_next(uintmax_t value) {
	if (!(rho_lt_3div4()
			? WholeNumSeqSBS1::write_next(value)
			: WholeNumSeqSBS2::write_next(value)))
		return false;
	
	update_rho(value);
	return true;
}

WholeNumSequence& WholeNumSequence::operator>>(uintmax_t& value) {
	if (rho_lt_3div4()) {
		WholeNumSeqSBS1::operator>>(value);
	} else {
		WholeNumSeqSBS2::operator>>(value);
	}
	update_rho(value);
	return *this;
}
WholeNumSequence& WholeNumSequence::operator<<(const uintmax_t& value) {
	if (rho_lt_3div4()) {
		WholeNumSeqSBS1::operator<<(value);
	} else {
		WholeNumSeqSBS2::operator<<(value);
	}
	update_rho(value);
	return *this;
}
