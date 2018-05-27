
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::rho_leq_pt75() const {
	return *reinterpret_cast<const char*>(&track_prev) != 0; 
}
template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::update_rho(uintmax_t n) {
	track_prev = {
			(unsigned)((n > 2) + (n >= 2)), 
			track_prev.a_nsub1_region <= 1
		};
}


template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::init(BitSequence<ENDIAN,false> bits) {
	this->WholeNumSeqBase<ENDIAN>::init(bits);
	track_prev = {2, false};
}



template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::has_next() const {
	return (rho_leq_pt75()
		? this->WholeNumSeqSBS1<ENDIAN>::has_next()
		: this->WholeNumSeqSBS2<ENDIAN>::has_next());
}
template <bool ENDIAN>
void WholeNumSequence<ENDIAN>::skip_next() {
	uintmax_t value;
	if (rho_leq_pt75()
			? this->WholeNumSeqSBS1<ENDIAN>::read_next(value)
			: this->WholeNumSeqSBS2<ENDIAN>::read_next(value)
		)
		update_rho(value);
}
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::fits_next(const uintmax_t& value) const {
	return (rho_leq_pt75()
		? this->WholeNumSeqSBS1<ENDIAN>::fits_next(value)
		: this->WholeNumSeqSBS2<ENDIAN>::fits_next(value));
}

template <bool ENDIAN>
inline bool WholeNumSequence<ENDIAN>::peek_next(uintmax_t& value) const {
	return (rho_leq_pt75()
		? this->WholeNumSeqSBS1<ENDIAN>::peek_next(value)
		: this->WholeNumSeqSBS2<ENDIAN>::peek_next(value));
}
template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::read_next(uintmax_t& value) {
	const bool successful = (rho_leq_pt75()
		? this->WholeNumSeqSBS1<ENDIAN>::read_next(value)
		: this->WholeNumSeqSBS2<ENDIAN>::read_next(value));
	// Update rho & return
	if (successful)
		update_rho(value);
	return successful;
}

template <bool ENDIAN>
bool WholeNumSequence<ENDIAN>::write_next(uintmax_t value) {
	const bool successful = (rho_leq_pt75()
		? this->WholeNumSeqSBS1<ENDIAN>::write_next(value)
		: this->WholeNumSeqSBS2<ENDIAN>::write_next(value));
	// Update rho & return
	if (successful)
		update_rho(value);
	return successful;
}

template <bool ENDIAN>
inline WholeNumSequence<ENDIAN>& WholeNumSequence<ENDIAN>::operator>>(uintmax_t& value) {
	if (rho_leq_pt75()) {
		this->WholeNumSeqSBS1<ENDIAN>::operator>>(value);
	} else {
		this->WholeNumSeqSBS2<ENDIAN>::operator>>(value);
	}
	update_rho(value);
	return *this;
}
template <bool ENDIAN>
inline WholeNumSequence<ENDIAN>& WholeNumSequence<ENDIAN>::operator<<(const uintmax_t& value) {
	if (rho_leq_pt75()) {
		this->WholeNumSeqSBS1<ENDIAN>::operator<<(value);
	} else {
		this->WholeNumSeqSBS2<ENDIAN>::operator<<(value);
	}
	update_rho(value);
	return *this;
}
