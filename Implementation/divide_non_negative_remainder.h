#ifndef DIVIDE_NON_NEGATIVE_REMAINDER_H
#define DIVIDE_NON_NEGATIVE_REMAINDER_H

#include <limits>

template <typename SINT>
void divide_w_nneg_remainder(const SINT& dividend, const SINT& divisor,
							 SINT& quotient, SINT& remainder) {
	quotient = dividend / divisor;
	remainder = dividend % divisor;
	if ((!std::numeric_limits<SINT>::is_specialized || std::numeric_limits<SINT>::is_signed) && remainder<0) {
		--quotient;
		remainder+=divisor;
	}
}
template <typename SINT>
SINT quotient_w_nneg_remainder(const SINT& dividend, const SINT& divisor) {
	SINT quotient, tmp;
	divide_w_pos_remainder(dividend, divisor, quotient, tmp);
	return quotient;
}
template <typename SINT>
SINT nneg_remainder(const SINT& dividend, const SINT& divisor) {
	SINT tmp, remainder;
	divide_w_pos_remainder(dividend, divisor, tmp, remainder);
	return remainder;
}

#endif
