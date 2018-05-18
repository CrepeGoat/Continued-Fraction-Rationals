
//////////////////////////////////////////
// COMMON FUNCTIONS / METHODS
//////////////////////////////////////////

template <typename MBYTE>
static void apply_bits(MBYTE bits_from, MBYTE& bits_to, MBYTE mask) {
	bits_to ^= (bits_from^bits_to) & mask;
}

// used to avoid un-portable behavior of negative numbers in shift operators
template <typename MBYTE, typename INTEGER>
static MBYTE shift_left(MBYTE bits, INTEGER n) {
	if (n >= 0) {
		return bits << n;
	} else {
		return bits >> -n;
	}
}

template <typename MBYTE, typename I1, typename I2>
static inline MBYTE mask(I1 offset, I2 length) {
	return ~(~MBYTE(0) << length) << offset;
}



//////////////////////////////////////////
// GLOBAL OPERATOR OVERLOADS
//////////////////////////////////////////

template <typename MBYTE, typename NBYTE, typename OPERATOR>
BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type> get_bitwise_operation(
	const BitSeqWord<MBYTE>& word_m,
	const BitSeqWord<NBYTE>& word_n,
	OPERATOR op
) {	
	if (word_m.BITS_PER_WORD > word_n.BITS_PER_WORD) {
		return get_bitwise_operation(word_n, word_m, op);
	}

	return BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type>(
		op(
			word_m.bits,
			shift_left(word_n.bits, word_m.bitmargin_lsb - word_n.bitmargin_lsb)
		),
		word_m.bitmargin_lsb,
		word_m.bitmargin_lsb + std::min(word_m.size(), word_n.size())
	);
}



template <typename MBYTE, typename NBYTE>
BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type> operator&(
	const BitSeqWord<MBYTE>& word_m, const BitSeqWord<NBYTE>& word_n)
{
	return get_bitwise_operation(word_m, word_n, []<typename T,typename U>(
		T bits_m, U bits_n) {
			return bits_m & bits_n;
		}
	);
}
template <typename MBYTE, typename NBYTE>
BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type> operator|(
	const BitSeqWord<MBYTE>& word_m, const BitSeqWord<NBYTE>& word_n)
{
	return get_bitwise_operation(word_m, word_n, []<typename T,typename U>(
		T bits_m, U bits_n) {
			return bits_m | bits_n;
		}
	);
}
template <typename MBYTE, typename NBYTE>
BitSeqWord<typename min_width<MBYTE, NBYTE>::value_type> operator^(
	const BitSeqWord<MBYTE>& word_m, const BitSeqWord<NBYTE>& word_n)
{
	return get_bitwise_operation(word_m, word_n, []<typename T,typename U>(
		T bits_m, U bits_n) {
			return bits_m ^ bits_n;
		}
	);
}

//////////////////////////////////////////
// BASE CLASS IMPLEMENTATION
//////////////////////////////////////////

template <typename MBYTE>
MBYTE BitSeqWordBase<MBYTE>::mask() const {
	return ~(~MBYTE(0) << size()) << bitmargin_lsb;
	// TODO fix this to work
	//return mask<MBYTE>(bitmargin_lsb, size());
}

template <typename MBYTE>
BitSeqWordBase<MBYTE>::BitSeqWordBase(
	typename BitSeqWordBase<MBYTE>::BitIndex_t begin,
	typename BitSeqWordBase<MBYTE>::BitIndex_t fromend
	) : bitmargin_lsb(begin),
		bitmargin_msb(fromend)
	{}

template <typename MBYTE>
typename BitSeqWordBase<MBYTE>::BitIndex_t BitSeqWordBase<MBYTE>::size() const {
	return this->BITS_PER_WORD - (bitmargin_lsb + bitmargin_msb); 
}








//////////////////////////////////////////
// VALUE DEFINITION & IMPLEMENTATION
//////////////////////////////////////////

/*

template <typename MBYTE>
BitSeqWord<MBYTE>& BitSeqWord<MBYTE>::operator<<=(
	typename BitSeqWordBase<MBYTE>::BitIndex_t n)
{
	shift_fwd(bits, n);
	if (n >= 0) {
		bitmargin_lsb = std::min(bitmargin_lsb + n, this->BITS_PER_WORD);
		bitmargin_msb = std::max(bitmargin_msb - n, 0);
	} else {
		bitmargin_lsb = std::max(bitmargin_lsb + n, 0);
		bitmargin_msb = std::min(bitmargin_msb - n, this->BITS_PER_WORD);
	}
	return *this;
}
//*/


template <typename MBYTE>
BitSeqWord<MBYTE>::BitSeqWord(
		MBYTE machine_word,
		std::size_t begin, 
		std::size_t end
	) : BitSeqWordBase<MBYTE>(begin, (end < this->BITS_PER_WORD) 
			? this->BITS_PER_WORD - end : 0),
		bits(machine_word)
	{}

template <typename MBYTE>
template <typename NBYTE>
BitSeqWord<MBYTE>::BitSeqWord(const BitSeqWord<NBYTE>& word)
	// Need to shift to least-significant bits, to avoid overflow in type conversion
	: BitSeqWordBase<MBYTE>(0, (word.size() < this->BITS_PER_WORD)
			? this->BITS_PER_WORD - word.size() : 0),
		bits(word.bits >> word.bitmargin_lsb)
	{}
template <typename MBYTE>
BitSeqWord<MBYTE>::BitSeqWord(const BitSeqWord<MBYTE>& word)
	// Can simply member-wise copy with same word type
	: BitSeqWordBase<MBYTE>(word.bitmargin_lsb, word.bitmargin_msb),
		bits(word.bits)
	{}
template <typename MBYTE>
BitSeqWord<MBYTE>::BitSeqWord(const BitSeqWord<MBYTE&>& word)
	// Can simply member-wise copy with same word type
	: BitSeqWordBase<MBYTE>(word.bitmargin_lsb, word.bitmargin_msb),
		bits(word.bits)
	{}


template <typename MBYTE>
template <typename NBYTE>
BitSeqWord<MBYTE>& BitSeqWord<MBYTE>::operator=(const BitSeqWord<NBYTE>& word) {
	if (word.BITS_PER_WORD <= this->BITS_PER_WORD
			|| word.bitmargin_msb >= word.BITS_PER_WORD - this->BITS_PER_WORD) {
		// Can simply member-wise copy with same word type
		this->bits = word.bits;
		this->bitmargin_lsb = word.bitmargin_lsb;
		this->bitmargin_msb = word.bitmargin_msb + this->BITS_PER_WORD - word.BITS_PER_WORD;
	} else {
		// Need to shift to least-significant bits, to avoid overflow in type conversion
		this->bits = word.bits >> word.bitmargin_lsb;
		this->bitmargin_lsb = 0;
		this->bitmargin_msb = (word.size() < this->BITS_PER_WORD)
			? this->BITS_PER_WORD - word.size() : 0;
	}
	return *this;
}



template <typename MBYTE>
MBYTE BitSeqWord<MBYTE>::get_bits() const {
	return (bits >> this->bitmargin_lsb)
		& mask<MBYTE>(0, this->size()); 
}

template <typename MBYTE>
BitSeqWord<MBYTE> BitSeqWord<MBYTE>::operator~() const {
	return BitSeqWord<MBYTE>(
		~bits, this->bitmargin_lsb, this->BITS_PER_WORD - this->bitmargin_msb);
}






//////////////////////////////////////////
// L-VALUE REFERENCE DEF & IMPLEMENTATION
//////////////////////////////////////////

template <typename MBYTE>
BitSeqWord<MBYTE&>::BitSeqWord(
		MBYTE& machine_word, 
		std::size_t begin, 
		std::size_t end
	) : BitSeqWordBase<MBYTE>(begin, (end < this->BITS_PER_WORD) 
			? this->BITS_PER_WORD - end : 0),
		bits(machine_word)
	{}


template <typename MBYTE>
template <typename NBYTE>
BitSeqWord<MBYTE&>& BitSeqWord<MBYTE&>::operator=(const BitSeqWord<NBYTE>& word) {	
	const MBYTE word_bits = (this->BITS_PER_WORD <= word.BITS_PER_WORD)
		? MBYTE(shift_left(word.bits, this->bitmargin_lsb - word.bitmargin_lsb))
		: shift_left(MBYTE(word.bits), this->bitmargin_lsb - word.bitmargin_lsb);
	if (this->size() > word.size()) {
		this->bitmargin_msb = this->BITS_PER_WORD - (word.size() + this->bitmargin_lsb);
	}
	apply_bits(word_bits, this->bits, this->mask());
	return *this;
}


template <typename MBYTE>
MBYTE BitSeqWord<MBYTE&>::get_bits() const {
	return (bits >> this->bitmargin_lsb)
		& mask<MBYTE>(0, this->size()); 
}

template <typename MBYTE>
BitSeqWord<MBYTE> BitSeqWord<MBYTE&>::operator~() const {
	return BitSeqWord<MBYTE>(
		~bits, this->bitmargin_lsb, this->BITS_PER_WORD - this->bitmargin_msb);
}


