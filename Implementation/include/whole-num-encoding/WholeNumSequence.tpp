template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::rho_lt_3div4() const {
	return rho_region < GEQ_3DIV4;
}
template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::update_rho(uintmax_t a_n) {
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


template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::init(BitSequence<ENDIAN,false> bits) {
	WholeNumSeqBase<ENDIAN>::init(bits);
	rho_region = EQ_0;
}



template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::has_next() const {
	return (rho_lt_3div4()
		? WholeNumSeqSBS1<ENDIAN>::has_next()
		: WholeNumSeqSBS2<ENDIAN>::has_next());
}
template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::skip_next() {
	uintmax_t value;
	if (rho_lt_3div4()
			? WholeNumSeqSBS1<ENDIAN>::read_next(value)
			: WholeNumSeqSBS2<ENDIAN>::read_next(value)
		)
		update_rho(value);
}
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::fits_next(const uintmax_t& value) const {
	return (rho_lt_3div4()
		? WholeNumSeqSBS1<ENDIAN>::fits_next(value)
		: WholeNumSeqSBS2<ENDIAN>::fits_next(value));
}

template <bool ENDIAN>
inline bool WholeNumSequence<ENDIAN>::peek_next(uintmax_t& value) const {
	return (rho_lt_3div4()
		? WholeNumSeqSBS1<ENDIAN>::peek_next(value)
		: WholeNumSeqSBS2<ENDIAN>::peek_next(value));
}
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::read_next(uintmax_t& value) {
	if (!(rho_lt_3div4()
			? WholeNumSeqSBS1<ENDIAN>::read_next(value)
			: WholeNumSeqSBS2<ENDIAN>::read_next(value)))
		return false;

	update_rho(value);
	return true;
}

template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::write_next(uintmax_t value) {
	if (!(rho_lt_3div4()
			? WholeNumSeqSBS1<ENDIAN>::write_next(value)
			: WholeNumSeqSBS2<ENDIAN>::write_next(value)))
		return false;
	
	update_rho(value);
	return true;
}

template <bool ENDIAN>
inline WholeNumSequence<ENDIAN>& WholeNumSequence<ENDIAN>::operator>>(uintmax_t& value) {
	if (rho_lt_3div4()) {
		WholeNumSeqSBS1<ENDIAN>::operator>>(value);
	} else {
		WholeNumSeqSBS2<ENDIAN>::operator>>(value);
	}
	update_rho(value);
	return *this;
}
template <bool ENDIAN>
inline WholeNumSequence<ENDIAN>& WholeNumSequence<ENDIAN>::operator<<(const uintmax_t& value) {
	if (rho_lt_3div4()) {
		WholeNumSeqSBS1<ENDIAN>::operator<<(value);
	} else {
		WholeNumSeqSBS2<ENDIAN>::operator<<(value);
	}
	update_rho(value);
	return *this;
}
