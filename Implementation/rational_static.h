#include "rational_base.h"
#include <bitset>

#ifndef RATIONAL_H
#define RATIONAL_H

/* TODO List:
 *		- make/use bitstream class for template-adjustable implementation
 *		- consider sizeless rational number class
 *
 */
namespace CFRN {
	typedef unsigned char T_index;

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

	template <unsigned char BYTE_COUNT>
	class T_rational_static : public T_rational_base {
	private:
		static const bool _0 = false;
		static const bool _1 = true;
		
		typedef unsigned char T_prefix;
		static const T_prefix MASK_NONZERO_RANGES=2;
		static const T_prefix MASK_NEG=1;	// NOTE: neg==1 -> the encoded value for '1'==bits(0)
		static const T_prefix _ZERO = 0;
		static const T_prefix _NEG1 = MASK_NEG;
		static const T_prefix _POS  = MASK_NONZERO_RANGES;
		static const T_prefix _NEG  = MASK_NONZERO_RANGES | MASK_NEG;
		
		// Internal Variables
		std::bitset<8*(BYTE_COUNT)> bits_enc;
		
		// INNER CLASSES
		//**********************************************************************
		class bitstream {
		private:
			std::bitset<8*(BYTE_COUNT)>& bits_enc;
			T_index index;
			
		public:
			struct pos_data {
				T_index index_tmp;
			};
			pos_data get_position_checkpoint() const {return index;}
			void restore_position(pos_data pos_checkpoint) {index = pos_checkpoint.index;}
			
			
			bitstream(T_rational_static& rational);
			bitstream(const bitstream& bstream);
			inline bitstream& operator=(const bitstream& bstream);
			
			
			bool has_next() const;
			bool has_next_multi(T_index bitcount) const;
			//inline void skip_next();
			//inline void skip_next_multi(T_rational::T_index bitcount);
			bitstream& operator>>(bool& bit);
			bitstream& operator<<(const bool bit);
			bitstream& get_multi(T_uint& target, T_index bitcount);
			bitstream& set_multi(const T_uint& source, T_index bitcount);
			
			void reset();
		};
		//**********************************************************************
		class T_intstream_static : public T_intostream_base {
		private:
			bitstream bstream;
			bool valid=false;
		public:
			T_intstream_static(T_rational_static& rational);
			T_intstream_static(const T_intstream_static& uintstream);
			operator bool() const;
			
			bool skip_next();
			bool has_next() const;
			bool peek_next(T_int& value) const;
			
			T_intstream_static& operator>>(T_int& uint_dec);
			T_intstream_static& operator<<(const T_uint& uint_dec);
			
			T_intstream_static& reset_and_set_1st(const T_int& int_dec);
			T_intstream_static& reset_and_get_1st(T_int& int_dec);
		};
		//**********************************************************************

		
		// Support Functions
		////////// TODO test //////////////
		static bool get_nth_digit_v1(const T_index digit, const T_uint& value);
		static bool get_nth_digit_v2(const T_index digit, const T_uint& value);
		static bool get_nth_digit_v3(const T_index digit, const T_uint& value);
		static inline bool get_nth_digit(const T_index digit, const T_uint& value);
		static inline void set_nth_digit(const T_index digit, T_uint& value, bool bit);
		static inline T_index log2(const T_uint& x);
		
		/*
		static inline T_rational::T_index no_of_bits_in_encoding_of(const T_uint_dec& value);
		static inline T_rational::T_uint_dec max_value_for_bitcount_of(T_index bitcount);
		inline bool can_encode_value_in_bitset_v1(const T_uint_dec& value_dec, T_index& i_enc) const;
		inline bool can_encode_value_in_bitset_v2(const T_uint_dec& value_dec, T_index& i_enc) const;
		inline bool can_encode_value_in_bitset(const T_uint_dec& value_dec, T_index& i_enc) const;
		*/
		
		
		// Utility Functions
		bool encode_into_bitstream(const T_int& value_dec, bitstream& bstream);
		bool decode_from_bitstream(bitstream& bstream, T_int& value_dec) const;
		void encode_1st_prefix_into_bitstream(const T_prefix prefix, bitstream& bstream);
		void decode_1st_prefix_from_bitstream(bitstream& bstream, T_prefix& prefix) const;
		
	public:
		// Constructors
		T_rational_static();
		//template <typename P_INT>
		//t_float_contfrac(P_INT p_begin, P_INT p_end);
		
		// Float Casting
		//operator float();
	};


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

}
#endif
