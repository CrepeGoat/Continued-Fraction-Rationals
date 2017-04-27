#ifndef RATIONAL_HYPEROPS_H
#define RATIONAL_HYPEROPS_H

#include "rational_base.h"
#include "divide_non_negative_remainder.h"
#include <utility>

/* TODO List:
 *		- make/use bitstream class for template-adjustable implementation
 *		- consider sizeless rational number class
 *
 */

namespace CFRN {
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

	class T_rational_unary_hyperops : public T_rational_base {
	protected:
		const T_rational_base &arg;
		const T_int vars_initial[4];
		static const unsigned char NUM_X=0, NUM_1=1, DEN_X=2, DEN_1=3;
		
		//////////////////////////////////////////////////////////////////
		class T_intostream_unary_hyperops : public T_intostream_base {
		protected:
			friend T_rational_unary_hyperops;
			T_intostream_base& arg_intostream;
			T_int vars[4];
			bool valid=false;
			
			template <typename INT>
			inline void apply_term(const INT& term) {
				std::swap(vars[NUM_X], vars[NUM_1]);
				std::swap(vars[DEN_X], vars[DEN_1]);
				vars[NUM_X] += vars[NUM_1]*term;
				vars[DEN_X] += vars[DEN_1]*term;
			}
			template <typename INT>
			inline bool produce_term(INT& div_x) {
				// assumes the denominator values are non-zero
				T_int rem_x, div_1, rem_1;
				divide_w_nneg_remainder(vars[NUM_X],vars[DEN_X],div_x,rem_x);
				divide_w_nneg_remainder(vars[NUM_1],vars[DEN_1],div_1,rem_1);
				if (div_x!=div_1)
					return false;
				vars[NUM_1] = vars[DEN_1];
				vars[NUM_X] = vars[DEN_X];
				vars[DEN_1] = rem_1;
				vars[DEN_X] = rem_x;
				return true;
			}
			
			//T_intostream_base& get_1st(T_int& int_dec);

		public:
			T_intostream_unary_hyperops(const T_rational_unary_hyperops& rational,
										T_int& value_s);
			T_intostream_unary_hyperops(const T_intostream_unary_hyperops& uintstream);
			~T_intostream_unary_hyperops();
			
			T_intostream_unary_hyperops* make_new_copy() const;
			
			operator bool();
			bool has_next() const;
			bool peek_next(T_uint& value) const;
			bool skip_next();
			
			T_intostream_unary_hyperops& operator>>(T_uint& value);
		};
		//////////////////////////////////////////////////////////////////
		
	public:
		T_intostream_unary_hyperops* make_new_uintstream(T_int& value_s) const;
		
		T_rational_unary_hyperops(const T_rational_base& x,
								  T_int n_x,
								  T_int n_1,
								  T_int d_x,
								  T_int d_1);
		
		T_rational_unary_hyperops* operator-() const;
		T_rational_unary_hyperops* inverse() const;
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
