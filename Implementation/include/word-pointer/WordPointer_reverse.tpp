////////////////////////////////////////////////////////////////////////////////
// INCREMENTATION METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T>
WordPointer<BYTE_PTR_T, false>&
WordPointer<BYTE_PTR_T, false>::operator++() {
	--b_ptr;
	return *this;
}

template <typename BYTE_PTR_T>
WordPointer<BYTE_PTR_T, false>
WordPointer<BYTE_PTR_T, false>::operator++(int) {
	return {b_ptr--};
}

template <typename BYTE_PTR_T>
WordPointer<BYTE_PTR_T, false>
WordPointer<BYTE_PTR_T, false>::operator+(std::size_t n) {
	return {b_ptr - n};
}

template <typename BYTE_PTR_T>
WordPointer<BYTE_PTR_T, false>&
WordPointer<BYTE_PTR_T, false>::operator+=(std::size_t n) {
	b_ptr -= n;
	return *this;
}



////////////////////////////////////////////////////////////////////////////////
// DECREMENTATION METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T>
WordPointer<BYTE_PTR_T, false>&
WordPointer<BYTE_PTR_T, false>::operator--() {
	++b_ptr;
	return *this;
}

template <typename BYTE_PTR_T>
WordPointer<BYTE_PTR_T, false>
WordPointer<BYTE_PTR_T, false>::operator--(int) {
	return {b_ptr++};
}

template <typename BYTE_PTR_T>
WordPointer<BYTE_PTR_T, false>
WordPointer<BYTE_PTR_T, false>::operator-(std::size_t n) {
	return {b_ptr + n};
}

template <typename BYTE_PTR_T>
WordPointer<BYTE_PTR_T, false>&
WordPointer<BYTE_PTR_T, false>::operator-=(std::size_t n) {
	b_ptr += n;
	return *this;
}




////////////////////////////////////////////////////////////////////////////////
// DISTANCE METHOD
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T>
std::ptrdiff_t WordPointer<BYTE_PTR_T, false>::operator-(const BYTE_PTR_T& ptr)
{
	return ptr - b_ptr;
}





////////////////////////////////////////////////////////////////////////////////
// DEREFERENCING METHODS
////////////////////////////////////////////////////////////////////////////////

template <typename BYTE_PTR_T>
uint8_t& WordPointer<BYTE_PTR_T, false>::operator*() {
	return *(b_ptr-1);
}

template <typename BYTE_PTR_T>
template <typename MWORD>
MWORD& WordPointer<BYTE_PTR_T, false>::deref() {
	return *((MWORD*) &*(b_ptr - sizeof(MWORD)));
}
