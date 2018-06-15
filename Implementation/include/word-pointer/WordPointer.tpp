////////////////////////////////////////////////////////////////////////////////
// INCREMENTATION METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>&
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator++() {
	if /*constexpr*/ (BYTES_FWD) {
		++b_ptr;
	} else {
		--b_ptr;
	}

	return *this;
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator++(int) {
	if /*constexpr*/ (BYTES_FWD)
		return b_ptr++;
	else
		return b_ptr--;
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator+(std::size_t n) {
	if /*constexpr*/ (BYTES_FWD)
		return {(b_ptr + n)};
	else
		return {(b_ptr - n)};
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>&
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator+=(std::size_t n) {
	if /*constexpr*/ (BYTES_FWD) {
		b_ptr += n;
	} else {
		b_ptr -= n;
	}

	return *this;
}



////////////////////////////////////////////////////////////////////////////////
// DECREMENTATION METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>&
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator--() {
	if /*constexpr*/ (BYTES_FWD) {
		--b_ptr;
	} else {
		++b_ptr;
	}

	return *this;
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator--(int) {
	if /*constexpr*/ (BYTES_FWD)
		return b_ptr--;
	else
		return b_ptr++;
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator-(std::size_t n) {
	if /*constexpr*/ (BYTES_FWD)
		return {(b_ptr - n)};
	else
		return {(b_ptr + n)};
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>&
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator-=(std::size_t n) {
	if /*constexpr*/ (BYTES_FWD) {
		b_ptr -= n;
	} else {
		b_ptr += n;
	}
	
	return *this;
}


////////////////////////////////////////////////////////////////////////////////
// DISTANCE METHOD
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T, bool BYTES_FWD>
std::ptrdiff_t WordPointer<BYTE_PTR_T, BYTES_FWD>::operator-(
		const BYTE_PTR_T& ptr_rhs
	)
{
	if /*constexpr*/ (BYTES_FWD)
		return b_ptr - ptr_rhs;
	else
		return ptr_rhs - b_ptr;
}




////////////////////////////////////////////////////////////////////////////////
// DEREFERENCING METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T, bool BYTES_FWD>
uint8_t& WordPointer<BYTE_PTR_T, BYTES_FWD>::operator*() {
	if /*constexpr*/ (BYTES_FWD)
		return *b_ptr;
	else
		return *(b_ptr-1);
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
template <typename MWORD>
MWORD& WordPointer<BYTE_PTR_T, BYTES_FWD>::deref() {
	if /*constexpr*/ (BYTES_FWD)
		return *((MWORD*) &*b_ptr);
	else
		return *((MWORD*) &*(b_ptr - sizeof(MWORD)));
}

