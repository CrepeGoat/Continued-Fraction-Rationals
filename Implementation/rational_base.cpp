#include "rational_base.h"
#include "rational_hyperops.h"
#include <functional>

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

/*
T_rational_base* T_rational_base::operator+(const T_rational_base& rhs) {
	return new T_rational_binary_ops(*this, rhs,
									  0,1,1,0,
									  0,0,0,1);
}
T_rational_base* T_rational_base::operator-(const T_rational_base& rhs) {
	return new T_rational_binary_ops(*this, rhs,
									  0,1,-1,0,
									  0,0,0,1);
}
T_rational_base* T_rational_base::operator*(const T_rational_base& rhs) {
	return new T_rational_binary_ops(*this, rhs,
									  1,0,0,0,
									  0,0,0,1);
}
T_rational_base* T_rational_base::operator/(const T_rational_base& rhs) {
	return new T_rational_binary_ops(*this, rhs,
									  0,1,0,0,
									  0,0,1,1);
}
T_rational_base* T_rational_base::inverse() {
	return new T_rational_binary_ops(*this, *this,
									  0,0,0,1,
									  0,1,0,0);
}
*/

template <template<typename> class R>
//, typename SINT, typename UINT>
inline bool evaluate_relation(const T_rational_base& lhs, const T_rational_base& rhs) {
	const R<T_int> relation_s{};
	T_int sint_l, sint_r;
	
	// Get first terms for comparison
	T_intostream_base& ios_l = *lhs.make_new_intstream(sint_l);
	T_intostream_base& ios_r = *rhs.make_new_intstream(sint_r);
	// Logic for failed stream output
	if (!(ios_l && ios_r)) {
		return false;
	} else if (sint_l != sint_r) {
		return relation_s(sint_l, sint_r);
	}
	
	const R<T_uint> relation_u{};
	T_uint uint_l, uint_r;
	bool reverse_return_value = true;
	while (true) {
		// Get next terms for comparison
		ios_l >> uint_l;
		ios_r >> uint_r;
		
		// Logic for failed stream output
		if (!(ios_l && ios_r)) {
			if (!(ios_l || ios_r)) {
				// both streams fail & previous values between streams are identical
				// -> represented rational numbers are identical
				uint_l = 1;
				uint_r = 1;
			} else {
				if (reverse_return_value!=ios_l) {
					uint_l = 1;
					uint_r = 2;
				} else {
					uint_l = 2;
					uint_r = 1;
				}
			}
			break;
		}
		
		// Logic for succesful stream output
		if (ios_l != ios_r) break;
		reverse_return_value = !reverse_return_value;
	}
	
RETURN:
	delete &ios_l;
	delete &ios_r;
	return (reverse_return_value? relation_u(uint_r,uint_l):relation_u(uint_l,uint_r));
}

bool operator<(const T_rational_base& lhs, const T_rational_base& rhs) {
	T_intostream_base
	*p_intstream_l = lhs.make_new_intstream(),
	*p_intstream_r = rhs.make_new_intstream();
	const bool ret = evaluate_relation<std::less<T_int> >(*p_intstream_l, *p_intstream_r);
	delete p_intstream_l;
	delete p_intstream_r;
	return ret;
}
bool operator>(const T_rational_base& lhs, const T_rational_base& rhs) {
	T_intostream_base
	*p_intstream_l = lhs.make_new_intstream(),
	*p_intstream_r = rhs.make_new_intstream();
	const bool ret = evaluate_relation<std::greater<T_int> >(*p_intstream_l, *p_intstream_r);
	delete p_intstream_l;
	delete p_intstream_r;
	return ret;
}
bool operator<=(const T_rational_base& lhs, const T_rational_base& rhs) {
	T_intostream_base
	*p_intstream_l = lhs.make_new_intstream(),
	*p_intstream_r = rhs.make_new_intstream();
	const bool ret = evaluate_relation<std::less_equal<T_int> >(*p_intstream_l, *p_intstream_r);
	delete p_intstream_l;
	delete p_intstream_r;
	return ret;
}
bool operator>=(const T_rational_base& lhs, const T_rational_base& rhs) {
	T_intostream_base
	*p_intstream_l = lhs.make_new_intstream(),
	*p_intstream_r = rhs.make_new_intstream();
	const bool ret = evaluate_relation<std::greater_equal<T_int> >(*p_intstream_l, *p_intstream_r);
	delete p_intstream_l;
	delete p_intstream_r;
	return ret;
}
bool operator==(const T_rational_base& lhs, const T_rational_base& rhs) {
	T_intostream_base
	*p_intstream_l = lhs.make_new_intstream(),
	*p_intstream_r = rhs.make_new_intstream();
	const bool ret = evaluate_relation<std::equal_to<T_int> >(*p_intstream_l, *p_intstream_r);
	delete p_intstream_l;
	delete p_intstream_r;
	return ret;
}
bool operator!=(const T_rational_base& lhs, const T_rational_base& rhs) {
	T_intostream_base
	*p_intstream_l = lhs.make_new_intstream(),
	*p_intstream_r = rhs.make_new_intstream();
	const bool ret = evaluate_relation<std::not_equal_to<T_int> >(*p_intstream_l, *p_intstream_r);
	delete p_intstream_l;
	delete p_intstream_r;
	return ret;
}

