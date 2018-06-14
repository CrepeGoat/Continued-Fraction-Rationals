#ifndef TYPEUTILITY_HPP
#define TYPEUTILITY_HPP

#include <cstdint>		// uintN_t
#include <type_traits>	// std::conditional
#include <limits>		// std::numeric_limits

template <typename MBYTE, typename MBYTE2>
using min_width = std::conditional<
	(sizeof(typename std::remove_reference<MBYTE>::type)
		<= sizeof(typename std::remove_reference<MBYTE2>::type)),
	typename std::remove_reference<MBYTE>::type,
	typename std::remove_reference<MBYTE2>::type
	>;

template <typename MBYTE, typename MBYTE2>
using max_width = std::conditional<
	(sizeof(typename std::remove_reference<MBYTE>::type)
		>= sizeof(typename std::remove_reference<MBYTE2>::type)),
	typename std::remove_reference<MBYTE>::type,
	typename std::remove_reference<MBYTE2>::type
	>;


template <typename INTEGER>
using to_signed =
	std::conditional<
		std::numeric_limits<INTEGER>::is_signed,
		INTEGER,
		typename std::conditional<
			std::is_same<INTEGER, uint8_t>::value,
			int8_t,
			typename std::conditional<
				std::is_same<INTEGER, uint16_t>::value,
				int16_t,
				typename std::conditional<
					std::is_same<INTEGER, uint32_t>::value,
					int32_t,
					typename std::conditional<
						std::is_same<INTEGER, uint64_t>::value,
						int64_t,
						void
					>::type
				>::type
			>::type
		>::type
	>;


#endif
