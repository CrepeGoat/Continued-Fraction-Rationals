#include "rational_hyperops.h"
using namespace CFRN;

/********************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ********************************************************************/

// Constructor & creation methods
T_rational_unary_hyperops::T_rational_unary_hyperops(const T_rational_base& x,
													 T_int n_x,
													 T_int n_1,
													 T_int d_x,
													 T_int d_1)
: arg(x), vars_initial{n_x, n_1, d_x, d_1} {}

T_rational_unary_hyperops::T_intostream_unary_hyperops* T_rational_unary_hyperops::make_new_uintstream(T_int& value_s) const {
	return new T_intostream_unary_hyperops(*this, value_s);
}

T_rational_unary_hyperops* T_rational_unary_hyperops::operator-() const {
	return new T_rational_unary_hyperops(arg,
										 -vars_initial[NUM_X], -vars_initial[NUM_1],
										 vars_initial[DEN_X], vars_initial[DEN_1]);
}

T_rational_unary_hyperops* T_rational_unary_hyperops::inverse() const {
	return new T_rational_unary_hyperops(arg,
										 vars_initial[DEN_X], vars_initial[DEN_1],
										 vars_initial[NUM_X], vars_initial[NUM_1]);
}

/********************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ********************************************************************/

// Constructors
T_rational_unary_hyperops::T_intostream_unary_hyperops::T_intostream_unary_hyperops(const T_rational_unary_hyperops& rational, T_int& value_s)
: vars{rational.vars_initial[0], rational.vars_initial[1], rational.vars_initial[2], rational.vars_initial[3]}, arg_intostream(*rational.arg.make_new_intstream(value_s)) {
	// Test validity based on *this state
	if ((vars[DEN_1]==0 && vars[DEN_1]==0)) {
		valid=false;
		return;
	}
	const T_int int_tmp = value_s;
	if ((vars[DEN_1]!=0 && vars[DEN_X]!=0) && produce_term(value_s)) {
		// ^ is true -> new term was created successfully
		apply_term(int_tmp);
		return;
	}
	
	// Else: get further term from argument
	if (!arg_intostream) {
		valid=false;
		return;
	}
	apply_term(int_tmp);
	T_uint uint_dec;
	while ((vars[DEN_1]==0 || vars[DEN_X]==0) || !produce_term(uint_dec)) {
		// ^ is true -> new term was created successfully
		// Else: get further term from argument
		if (!arg_intostream >> uint_dec) {
			valid=false;
			break;
		}
		apply_term(uint_dec);
	}
}
T_rational_unary_hyperops::T_intostream_unary_hyperops::T_intostream_unary_hyperops(const T_intostream_unary_hyperops& intostream)
: vars{intostream.vars[0],intostream.vars[1],intostream.vars[2],intostream.vars[3]}, arg_intostream(*intostream.make_new_copy()) {}
T_rational_unary_hyperops::T_intostream_unary_hyperops::~T_intostream_unary_hyperops() {
	delete &arg_intostream;
}


// Public methods
T_rational_unary_hyperops::T_intostream_unary_hyperops::operator bool() {
	return valid;
}
bool T_rational_unary_hyperops::T_intostream_unary_hyperops::has_next() const {
	T_intostream_unary_hyperops tmp (*this);
	return tmp.skip_next();
}
bool T_rational_unary_hyperops::T_intostream_unary_hyperops::peek_next(T_uint& value) const {
	return T_intostream_unary_hyperops(*this).operator>>(value);
}
bool T_rational_unary_hyperops::T_intostream_unary_hyperops::skip_next() {
	T_uint tmp;
	return operator>>(tmp);
}

T_rational_unary_hyperops::T_intostream_unary_hyperops& T_rational_unary_hyperops::T_intostream_unary_hyperops::operator>>(T_uint& uint_dec) {
	// Test validity based on *this state
	if ((vars[DEN_1]==0 && vars[DEN_1]==0)) {
		valid=false;
		return *this;
	}
	while ((vars[DEN_1]==0 || vars[DEN_X]==0) || !produce_term(uint_dec)) {
		// ^ is true -> new term was created successfully
		// Else: get further term from argument
		if (!arg_intostream >> uint_dec) {
			valid=false;
			return *this;
		}
		apply_term(uint_dec);
	}
	return *this;
}
