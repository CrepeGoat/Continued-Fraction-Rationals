#ifndef BITTWIDDLES_HPP
#define BITTWIDDLES_HPP

#include <cstdint>	// uint8_t


// used to avoid un-portable behavior of negative numbers in shift operators
template <typename MBYTE, typename INTEGER>
MBYTE shift_left(MBYTE bits, INTEGER n);

template <typename MBYTE, typename INTEGER>
MBYTE shift_right(MBYTE bits, INTEGER n);

template <typename MBYTE>
void apply_bits(MBYTE bits_from, MBYTE& bits_to, MBYTE mask);

template <typename MBYTE, typename I1, typename I2>
MBYTE mask(I1 offset, I2 length);



template <typename MBYTE>
MBYTE lsb(MBYTE value);

template <typename MBYTE>
MBYTE msb(MBYTE value);



template <typename MBYTE>
uint8_t bit_pos_0h(MBYTE value);

template <typename MBYTE>
int8_t bit_pos_0l(MBYTE value);

template <typename MBYTE>
int8_t bit_pos(MBYTE value);



#include "BitTwiddles.tpp"

#endif
