#include "BitSequence.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "BitTwiddles.h"

//using namespace BitSequence_NS;

static const std::size_t SIZE = 16;
typedef BitSequence<true,false> BitSeq_t;

void cout_header(
	std::string str_suite,
	std::string str_target, 
	std::string str_testtype)
{
	std::cout << "Test suite:\t" << str_suite << std::endl;
	std::cout << "\ttarget:\t" << str_target << std::endl;
	std::cout << "\ttype:\t" << str_testtype << std::endl;
}

/*******************************************************************************
 * TESTS - OPERATORS <<, >>
 ******************************************************************************/
void BitSequence_test_inout_stress() {
	cout_header("BitSequence class", "operator<<, operator>> methods", "stress test");

	std::cout << "Allocating " << SIZE << " bytes in memory... ";
	byte storage[SIZE]{};
	std::cout << "done." << std::endl;

	bool test_status = true;
	BitSeq_t bstream;
	std::size_t pos_bit;
	std::string bits_copy1, bits_copy2;
	bool bit_next;

	std::cout << std::noboolalpha;
	while (test_status) {
		pos_bit = 0;
		bstream.init(storage,storage+SIZE);
		bits_copy1.clear();
		while (test_status && pos_bit < CHAR_BIT*SIZE) {
			bit_next = rand()%2;
			bstream << bit_next;
			bits_copy1.push_back(bit_next ? '1':'0');
			pos_bit += 1;
		}

		pos_bit = 0;
		bstream.init(storage,storage+SIZE);
		bits_copy2.clear();
		while (bstream.has_next() && pos_bit < CHAR_BIT*SIZE) {
			bstream >> bit_next;
			bits_copy2.push_back(bit_next ? '1':'0');
			pos_bit += 1;
		}

		std::cout << "Bits in  = " << bits_copy1 << std::endl;
		if (bits_copy1 == bits_copy2) {
			std::cout << "Success! (Bits out = bits in)" << std::endl;
		} else {
			test_status = false;	// Test case failed!
			std::size_t i;
			std::cout << "Bits out = " << bits_copy2 << std::endl;
			std::cout << "Diffs   -> ";
			for (i=0; i<std::min(bits_copy1.length(), bits_copy2.length()); ++i)
				std::cout << (bits_copy1[i] == bits_copy2[i] ? " " : "^");
			for (; i<std::max(bits_copy1.length(), bits_copy2.length()); ++i)
				std::cout << "^";
			std::cout << std::endl;

			std::cout << "Bits stored = ";
			for (std::size_t i=0; i<SIZE; ++i) {
				std::cout << std::hex << (unsigned short)(storage[i]);
			}
			std::cout << std::dec << std::endl;
		}
	}
	std::cout << std::boolalpha << "Ending Test." << std::endl;
	return;
}


/*******************************************************************************
 * TESTS - METHODS get_streak, set_streak
 ******************************************************************************/
void BitSequence_test_getsetstreak_manual() {
	cout_header("BitSequence class", "get_streak, set_streak methods",
		"manual input case test");

	std::cout << "Allocating " << SIZE << " bytes in memory... ";
	byte storage[SIZE]{};
	std::cout << "done." << std::endl;

	std::cout << "Instantiating BitSequence object... ";
	BitSeq_t bstream(storage,storage+SIZE);
	std::cout << "done.\n" << std::endl;

	std::size_t i=0;
	std::ptrdiff_t i_tmp;
	std::size_t j_tmp;
	std::string bits_copy1;
	while (true) {
		if (i>=CHAR_BIT*SIZE) {
			std::cout << "Storage capacity reached. Terminating input loop."
					<< std::endl;
			break;
		}
		std::cout << "(Empty bits remaining: " << CHAR_BIT*SIZE-i << ")\n";
		std::cout << "Repetition length of bit to add = ";
		std::cin.clear();
		std::cin >> i_tmp;
		//if (!) {
		//	std::cout << "\nInvalid input" << std::endl;
		//	continue;
		//}
		//std::cout << std::endl;
		i_tmp = std::min<std::ptrdiff_t>(i_tmp, CHAR_BIT*SIZE-i);
		if (i_tmp <= 0) {
			std::cout << "Terminating input loop." << std::endl;
			break;
		}
		std::cout << "Bit to add = ";
		std::cin.clear();
		std::cin >> j_tmp;
		//if (!std::cin >> j_tmp) {
		//	std::cout << "\nInvalid input" << std::endl;
		//	continue;
		//}
		//std::cout << std::endl;

		bstream.set_streak(j_tmp, i_tmp);
		bits_copy1.append(i_tmp, j_tmp ? '1':'0');
		i += i_tmp;
	}

	std::cout << "Outputting bits from BitSequence... " << std::endl;

	bstream.init(storage,storage+SIZE);
	std::string bits_copy2;
	std::size_t s_tmp;
	bool b_tmp=true;
	while (bstream.has_next() && i>0) {
		s_tmp = bstream.get_streak(b_tmp,i);
		bits_copy2.append(s_tmp,b_tmp ? '1':'0');
		i-=s_tmp;
		b_tmp = !b_tmp;
	}

	std::cout << "Bits in  = " << bits_copy1 << std::endl;
	if (bits_copy1 == bits_copy2) {
		std::cout << "Success! (Bits out = bits in)" << std::endl;
	} else {
		std::cout << "Bits out = " << bits_copy2 << std::endl;
	}

	std::cout << "Ending Test." << std::endl;
	return;
}


void BitSequence_test_getsetstreak_stress() {
	cout_header("BitSequence class", "get/set_streak methods", "stress test");

	std::cout << "Allocating " << SIZE << " bytes in memory... ";
	byte storage[SIZE]{};
	std::cout << "done." << std::endl;

	bool test_status = true;
	BitSeq_t bstream;
	std::size_t pos_bit;
	std::size_t len_bitrun;
	std::string bits_copy1, bits_copy2;
	bool bit_next = true;

	std::cout << std::noboolalpha << "Begin test." << std::endl;
	while (test_status) {
		std::cout << "Setting bits in collections of monotone sequences...";
		pos_bit = 0;
		bstream.init(storage,storage+SIZE);
		bits_copy1.clear();
		while (test_status && pos_bit < CHAR_BIT*SIZE) {
			len_bitrun = std::min<std::size_t>(1+rand()%10, CHAR_BIT*SIZE - pos_bit);

			bstream.set_streak(bit_next, len_bitrun);
			bits_copy1.append(len_bitrun, bit_next ? '1':'0');

			bit_next = !bit_next;
			pos_bit += len_bitrun;
		}
		std::cout << "done." << std::endl;

		std::cout << "Reading bits in collections of monotone sequences...";
		pos_bit = 0;
		bstream.init(storage, storage+SIZE);
		bits_copy2.clear();
		while (bstream.has_next() && pos_bit < CHAR_BIT*SIZE) {
			len_bitrun = bstream.get_streak(bit_next);
			bits_copy2.append(len_bitrun, bit_next ? '1':'0');
			pos_bit += len_bitrun;
			bit_next = !bit_next;
		}
		std::cout << "done." << std::endl;

		std::cout << "Bits in  = " << bits_copy1 << std::endl;
		if (bits_copy1 == bits_copy2) {
			std::cout << "Success! (Bits out = bits in)" << std::endl;
		} else {
			test_status = false;	// Test case failed!
			std::size_t i;
			std::cout << "Bits out = " << bits_copy2 << std::endl;
			std::cout << "Diffs   -> ";
			for (i=0; i<std::min(bits_copy1.length(), bits_copy2.length()); ++i)
				std::cout << (bits_copy1[i] == bits_copy2[i] ? " " : "^");
			for (; i<std::max(bits_copy1.length(), bits_copy2.length()); ++i)
				std::cout << "^";
			std::cout << std::endl;

			std::cout << "Bits stored = ";
			for (std::size_t i=0; i<SIZE; ++i) {
				std::cout << std::hex << (unsigned short)(storage[i]);
			}
			std::cout << std::dec << std::endl;
		}
	}
	std::cout << std::boolalpha << "Ending Test." << std::endl;
	return;
}

/*******************************************************************************
 * TESTS - METHODS get_to_int, set_from_int
 ******************************************************************************/
void BitSequence_test_setfromint_predefrange() {
	cout_header("BitSequence class", "set_from_int methods", "pre-defined input range");

	BitSeq_t bseq;
	byte storage, n;
	std::size_t bitlen;
	bool bit;

	std::cout << "Beginning test." << std::endl;
	for (n=1; n != 0; ++n) {
		bitlen = bit_pos_0l(msb(n))+1;
		std::cout << "Getting " << bitlen << " bits from int... ";
		bseq.init(&storage, &storage+sizeof(byte));
		bseq.set_from_int(n, bitlen);
		std::cout << "done." << std::endl;

		std::cout << "n = " << (unsigned long long)n << "\tbseq = ";
		bseq.init(&storage, &storage+sizeof(byte));
		while(bitlen-- > 0) {
			bseq >> bit;
			std::cout << (bit ? '1':'0');
		}
		std::cout << std::endl;
	}

	std::cout << "Ending Test." << std::endl;
	return;
}

template <typename MBYTE>
void BitSequence_test_getsetint_predefrange() {
	cout_header("BitSequence class", "get_to_int, set_from_int methods",
		"pre-defined input range");

	BitSeq_t bseq;
	byte storage[sizeof(MBYTE)+1]{};
	MBYTE n, n2;
	std::size_t bitlen, offset;
	bool bit;

	std::cout << "Beginning test." << std::endl;
	for (n=1; n != 0; ++n) {
		std::cout << "n  = " << (unsigned long long)n << std::endl;
		bitlen = bit_pos_0l(msb(n))+1;
		offset = rand() % CHAR_BIT;
		std::cout << "Setting " << bitlen << " bits from int... ";
		bseq.init(storage, storage+sizeof(MBYTE)+1);
		bseq.skip_next(offset);
		bseq.set_from_int(n, bitlen);
		std::cout << "done." << std::endl;

		std::cout << "Setting new int bits from storage... ";
		n2 = 0;
		bseq.init(storage, storage+sizeof(MBYTE)+1);
		bseq.skip_next(offset);
		bseq.get_to_int(n2, bitlen);
		std::cout << "done." << std::endl;

		if (n == n2) {
			std::cout << "Pass! (new int == old int)" << std::endl;
		} else {
			std::cout << "Failure!" << std::endl;
			std::cout << "n2 = " << n2 << std::endl;
			std::cout << "storage bits = ";
			bseq.init(storage, storage+sizeof(MBYTE)+1);
			bseq.skip_next(offset);
			while(bitlen-- > 0) {
				bseq >> bit;
				std::cout << (bit ? '1':'0');
			}
			std::cout << std::endl;
			break;
		}
	}

	std::cout << "Ending Test." << std::endl;
	return;
}

/*******************************************************************************
 * TESTS - METHOD set_from
 ******************************************************************************/
void BitSequence_test_setfrom_stress() {
	cout_header("BitSequence class", "set_from  method", "stress test");

 	std::cout << "Allocating " << SIZE << " bytes in memory... ";
 	byte storage[SIZE]{};
 	std::cout << "done." << std::endl;

 	bool test_status = true;
	BitSeq_t bstream, bstream2;
	std::size_t i, pos1_1, pos1_2, pos2;
	std::string bits_copy_tmp, bits_copy1, bits_copy2;
	bool bit_next;

	// Populate stream with random bits
	bstream.init(storage,storage+SIZE);
	for (i=0; i < CHAR_BIT*SIZE; ++i) {
		bit_next = rand()%2;
		bstream << bit_next;
	}
	// Run stress test
	std::cout << std::noboolalpha;
	while (test_status) {
		// Choose a random source range & destination
		pos1_1 = rand() % (CHAR_BIT*SIZE);
		pos1_2 = rand() % (CHAR_BIT*SIZE);
		if (pos1_1 > pos1_2)
			std::swap(pos1_1,pos1_2);
		else
			++pos1_2;
		pos2 = rand() % (CHAR_BIT*SIZE+1 - (pos1_2-pos1_1));
		std::cout << "Copy range source  = [" << pos1_1 << ", " << pos1_2 << ")"
				<< std::endl;
		std::cout << "Copy position dest = [" << pos2 << ", "
				<< pos2 + pos1_2-pos1_1 << ")" << std::endl;
		// Directly check current bits
		bstream.init(storage,storage+SIZE);
		for (i=0; i < CHAR_BIT*SIZE; ++i) {
			bstream >> bit_next;
			bits_copy1.push_back(bit_next ? '1':'0');
		}
		std::cout << "Source bits          = " << bits_copy1 << std::endl;
		// Assign bits from stream 1 to 2
		bits_copy_tmp = bits_copy1;
		for (i=0; i < pos1_2-pos1_1; ++i) {
			bits_copy1[pos2+i] = bits_copy_tmp[pos1_1+i];
 		}
		std::cout << "Expected result bits = " << bits_copy1 << std::endl;
		bstream.init(storage,storage+SIZE);
		bstream.skip_next(pos1_1);
		bstream2.init(storage,storage+SIZE);
		bstream2.skip_next(pos2);
		bstream2.set_from(bstream, pos1_2-pos1_1);
		// Directly check resulting bitstream contents
		bstream.init(storage,storage+SIZE);
		for (i=0; i < CHAR_BIT*SIZE; ++i) {
			bstream >> bit_next;
			bits_copy2.push_back(bit_next ? '1':'0');
		}

		// Check results
 		if (bits_copy1 == bits_copy2) {
 			std::cout << "Success! (Expected result matches true result)\n"
					<< std::endl;
			// Populate previous source range with random bits
			bstream.init(storage,storage+SIZE);
			bstream.skip_next(pos1_1);
			for (i=pos1_1; i < pos1_2; ++i) {
				bit_next = rand()%2;
	 			bstream << bit_next;
	 		}
 		} else {
 			test_status = false;	// Test case failed!
			std::cout << "True result bits     = " << bits_copy2 << std::endl;
 			std::cout << "Diffs               -> ";
 			for (i=0; i<std::min(bits_copy1.length(), bits_copy2.length()); ++i)
 				std::cout << (bits_copy1[i] == bits_copy2[i] ? " " : "^");
 			for (; i<std::max(bits_copy1.length(), bits_copy2.length()); ++i)
 				std::cout << "^";
 			std::cout << std::endl;

 			std::cout << "Bytes stored = ";
 			for (i=0; i<SIZE; ++i) {
 				std::cout << std::hex << (unsigned short)(storage[i]);
 			}
 			std::cout << std::dec << std::endl << std::endl;
 		}
		bits_copy1.clear();
		bits_copy2.clear();
 	}
 	std::cout << std::boolalpha << "Ending Test." << std::endl;
 	return;
 }


int main() {
	//BitSequence_test_getsetstreak_manual();

	//BitSequence_test_inout_stress();
	//BitSequence_test_getsetstreak_stress();
	//BitSequence_test_setfrom_stress();
	//BitSequence_test_setfromint_predefrange();
	//BitSequence_test_getsetint_predefrange<unsigned short>();
}
