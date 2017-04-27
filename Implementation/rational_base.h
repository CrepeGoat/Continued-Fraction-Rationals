#ifndef RATIONAL_BASE_H
#define RATIONAL_BASE_H

/* TODO List:
 *		- make/use bitstream class for template-adjustable implementation
 *		- consider sizeless rational number class
 *
 */

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

// i.e. Continued-Fraction Rational Numbers
namespace CFRN {
	
	// Declaration
	class T_intostream_base;
	
	// Typedefs (TODO may be removed for template implementations later)
	typedef long int T_int;
	typedef unsigned long T_uint;
	
	// Classes
	//////////////////////////////////////////////////////////////////
	class T_rational_base {
	protected:
		friend T_intostream_base;
		
	public:
		virtual T_intostream_base* make_new_intstream(T_int& value_s) const=0;
		
		virtual T_rational_base* operator-() const =0;
		virtual T_rational_base* inverse() const;
		
	};
	//////////////////////////////////////////////////////////////////
	class T_intostream_base {
	protected:
		// Is called in constructor
		//virtual T_intostream_base& get_1st(T_int& int_dec)=0;
	
	public:
		virtual ~T_intostream_base()=0;
		
		virtual T_intostream_base* make_new_copy() const=0;
		
		virtual operator bool()=0;
		virtual bool has_next() const=0;
		//virtual bool peek_next(T_uint& value) const=0;
		virtual bool peek_next(T_uint& value) const=0;
		virtual bool skip_next()=0;
		
		//virtual T_intostream_base& operator>>(T_uint& value)=0;
		virtual T_intostream_base& operator>>(T_uint& value)=0;
	};
	//////////////////////////////////////////////////////////////////

}

//virtual T_rational_base* operator+(const T_rational_base& rhs) const;
//virtual T_rational_base* operator-(const T_rational_base& rhs) const;
//virtual T_rational_base* operator*(const T_rational_base& rhs) const;
//virtual T_rational_base* operator/(const T_rational_base& rhs) const;
//virtual T_rational_base& operator%(const T_rational_base& rhs) const=0;

bool operator< (const CFRN::T_rational_base& lhs, const CFRN::T_rational_base& rhs);
bool operator> (const CFRN::T_rational_base& lhs, const CFRN::T_rational_base& rhs);
bool operator<=(const CFRN::T_rational_base& lhs, const CFRN::T_rational_base& rhs);
bool operator>=(const CFRN::T_rational_base& lhs, const CFRN::T_rational_base& rhs);
bool operator==(const CFRN::T_rational_base& lhs, const CFRN::T_rational_base& rhs);
bool operator!=(const CFRN::T_rational_base& lhs, const CFRN::T_rational_base& rhs);

#endif
