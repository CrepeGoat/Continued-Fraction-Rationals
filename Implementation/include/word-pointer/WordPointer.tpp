////////////////////////////////////////////////////////////////////////////////
// INCREMENTATION METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>&
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator++() {
	++b_ptr;
	return *this;
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator++(int) {
	return {b_ptr++};
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator+(std::size_t n) {
	return {b_ptr + n};
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>&
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator+=(std::size_t n) {
	b_ptr += n;
	return *this;
}



////////////////////////////////////////////////////////////////////////////////
// DECREMENTATION METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>&
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator--() {
	--b_ptr;
	return *this;
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator--(int) {
	return {b_ptr--};
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator-(std::size_t n) {
	return {b_ptr - n};
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
WordPointer<BYTE_PTR_T, BYTES_FWD>&
WordPointer<BYTE_PTR_T, BYTES_FWD>::operator-=(std::size_t n) {
	b_ptr -= n;
	return this;
}


////////////////////////////////////////////////////////////////////////////////
// DISTANCE METHOD
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T, bool BYTES_FWD>
std::ptrdiff_t WordPointer<BYTE_PTR_T, BYTES_FWD>::operator-(
		const BYTE_PTR_T& ptr
	)
{
	return b_ptr - ptr;
}




////////////////////////////////////////////////////////////////////////////////
// DEREFERENCING METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T, bool BYTES_FWD>
uint8_t& WordPointer<BYTE_PTR_T, BYTES_FWD>::operator*() {
	return *b_ptr;
}

template <typename BYTE_PTR_T, bool BYTES_FWD>
template <typename MWORD>
MWORD& WordPointer<BYTE_PTR_T, BYTES_FWD>::deref() {
	return *((MWORD*) &*b_ptr);
}

