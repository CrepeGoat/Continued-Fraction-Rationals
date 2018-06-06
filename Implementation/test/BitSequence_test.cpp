#include "BitSequence.hpp"

#include "BitTwiddles.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <limits>

//using namespace BitSequence_NS;

static const std::size_t SIZE = 16;
static constexpr bool ENDIAN = true;

typedef BitSequence<ENDIAN,false> BitSeq_t;

void cout_header(
	std::string str_suite,
	std::string str_target, 
	std::string str_testtype)
{
	std::cout << "==================================================\n"
		<< "Test suite:\t" << str_suite << std::endl
		<< "\ttarget:\t" << str_target << std::endl
		<< "\ttype:\t" << str_testtype << std::endl;
}


std::string diffs(const std::string& str1, const std::string& str2) {
	std::string diffs;
	std::size_t i;
	
	for (i=0; i<std::min(str1.length(), str2.length()); ++i) {
		diffs.push_back(str1[i] == str2[i] ? ' ' : '^');
	}
	for (; i<std::max(str1.length(), str2.length()); ++i) {
		diffs.push_back('^');
	}
	
	return diffs;
}

/*******************************************************************************
 * TESTS - OPERATORS <<, >>
 ******************************************************************************/
template <typename BitSeq_t>
void BitSequence_test_inout_stress(
		std::size_t storage_size = 1,
		std::size_t max_test_count = -1
	)
{
	cout_header(
		"BitSequence class", 
		"operator<<, operator>> methods", 
		"stress test"
		);

	std::vector<uint8_t> storage(storage_size, 0xFF);

	BitSeq_t bseq;
	std::size_t pos_bit;
	std::string bits_strcopy1, bits_strcopy2;
	bool bit_next;

	std::cout << std::noboolalpha;

	std::size_t test_counter;
	for (test_counter = 0; test_counter < max_test_count; ++test_counter) {
		pos_bit = 0;
		bseq.init(storage.data(), storage.data()+storage.size());
		bits_strcopy1.clear();
		while (pos_bit < CHAR_BIT*storage.size()) {
			bit_next = rand()%2;
			bseq << bit_next;
			bits_strcopy1.push_back(bit_next ? '1':'0');
			pos_bit += 1;
		}

		pos_bit = 0;
		bseq.init(storage.data(), storage.data()+storage.size());
		bits_strcopy2.clear();
		while (bseq.has_next() && pos_bit < CHAR_BIT*storage.size()) {
			bseq >> bit_next;
			bits_strcopy2.push_back(bit_next ? '1':'0');
			pos_bit += 1;
		}

		if (bits_strcopy1 != bits_strcopy2) {
			throw std::logic_error(
					std::string("read/write bits do not match;\n")
					+ bits_strcopy1 + std::string("\n")
					+ bits_strcopy2 + std::string("\n")
					+ diffs(bits_strcopy1, bits_strcopy2)
				);
		}
	}
	std::cout << std::boolalpha << test_counter << " tests passed!" << std::endl;
	return;
}


/*******************************************************************************
 * TESTS - METHODS read_streak, write_streak
 ******************************************************************************/
template <typename BitSeq_t>
void test_BitSequence_readwritestreak_single(
	const std::vector<std::size_t>& streaks,
	std::size_t storage_size
	)
{
	static std::vector<uint8_t> storage;
	static std::vector<std::size_t> streaks_copy;
	static BitSeq_t bseq;

	storage.assign(storage_size, 0xFF);
	streaks_copy.clear();
	streaks_copy.reserve(streaks.size());

	bseq.init(storage.data(), storage.data()+storage.size());
	for (std::size_t i=0; i < streaks.size(); ++i) {
		bseq.write_streak(i%2, streaks[i]);
	}

	bseq.init(storage.data(), storage.data()+storage.size());
	while (bseq.has_next()) {
		streaks_copy.push_back(
			bseq.read_streak(streaks_copy.size() % 2));
	}

	if (!std::equal(streaks.begin(), streaks.end()-1, streaks_copy.begin())
		|| CHAR_BIT*storage_size != std::accumulate(
			streaks_copy.begin(), streaks_copy.end(), 0))
	{
		throw std::logic_error(
				std::string("read/write bit streaks do not match;\n")
			);
	}
}

template <typename BitSeq_t>
void BitSequence_test_readwritestreak_stress(
		std::size_t storage_size = 1,
		std::size_t max_test_count = -1
	)
{
	cout_header(
			"BitSequence class",
			"read/write_streak methods",
			"stress test"
		);

	std::vector<uint8_t> storage(storage_size, 0xFF);

	BitSeq_t bseq;
	std::size_t len_bitrun;
	std::vector<std::size_t> streaks;

	std::cout << std::noboolalpha;
	
	std::size_t test_counter;
	for (test_counter = 0; test_counter < max_test_count; ++test_counter) {
		// Randomizes initial bit in sequence
		if (rand() % 2) {
			streaks.push_back(0);
		}

		for (std::size_t pos_bit=0; pos_bit < CHAR_BIT*storage.size();
			pos_bit += len_bitrun)
		{
			len_bitrun = 1 + rand()%10;
			streaks.push_back(len_bitrun);
		}

		test_BitSequence_readwritestreak_single<BitSeq_t>(streaks, storage_size);
		streaks.clear();
	}
	std::cout << std::boolalpha << test_counter << " tests passed!" << std::endl;
	return;
}

/*******************************************************************************
 * TESTS - METHODS read_to_int, write_from_int
 ******************************************************************************/
template <typename BitSeq_t>
void BitSequence_test_writefromint_predefrange() {
	cout_header(
			"BitSequence class",
			"write_from_int methods",
			"pre-defined input range"
		);

	BitSeq_t bseq;
	uint8_t storage, n;
	std::size_t bitlen;
	bool bit;

	//std::cout << "Beginning test." << std::endl;
	for (n=1; n != 0; ++n) {
		bitlen = bit_pos_0l(msb(n))+1;
		//std::cout << "Getting " << bitlen << " bits from int... ";
		bseq.init(&storage, &storage+sizeof(uint8_t));
		bseq.write_from_int(n, bitlen);
		//std::cout << "done." << std::endl;

		//std::cout << "n = " << (unsigned long long)n << "\tbseq = ";
		bseq.init(&storage, &storage+sizeof(uint8_t));
		while(bitlen-- > 0) {
			bseq >> bit;
		//	std::cout << (bit ? '1':'0');
		}
		std::cout << std::endl;
	}

	std::cout << "Ending Test." << std::endl;
	return;
}

template <typename BitSeq_t, typename MBYTE>
void BitSequence_test_readwriteint_predefrange(
		std::size_t storage_size = 1
	) {
	cout_header(
			"BitSequence class",
			"read_to_int, write_from_int methods",
			"pre-defined input range"
		);

	BitSeq_t bseq;
	std::vector<uint8_t> storage(storage_size, 0xFF);
	MBYTE n, n2;
	std::size_t bitlen, offset;
	bool bit;

	for (n=1; n != 0; ++n) {
		bitlen = bit_pos_0l(msb(n))+1;
		offset = rand() % CHAR_BIT;

		bseq.init(storage.data(), storage.data()+storage.size());
		bseq.skip_next(offset);
		bseq.write_from_int(n, bitlen);

		n2 = 0;
		bseq.init(storage.data(), storage.data()+storage.size());
		bseq.skip_next(offset);
		bseq.read_to_int(n2, bitlen);

		if (n != n2) {
			throw std::logic_error(
					std::string("read/write bits do not match;\n")
					//+ std::string(n) + std::string("\n")
					//+ std::string(n2) + std::string("\n")
				);
			//std::cout << "storage bits = ";
			//bseq.init(storage, storage+sizeof(MBYTE)+1);
			//bseq.skip_next(offset);
			//while(bitlen-- > 0) {
			//	bseq >> bit;
			//	std::cout << (bit ? '1':'0');
			//}
			//std::cout << std::endl;
			//break;
		}
	}

	std::cout << MBYTE(-1) + 0u << " tests passed." << std::endl;
	return;
}

/*******************************************************************************
 * TESTS - METHOD write_from
 ******************************************************************************/
template <typename BitSeq_t>
void BitSequence_test_writefrom_stress(
		std::size_t storage_size = 1,
		std::size_t max_test_count = -1
	)
{
	cout_header("BitSequence class", "write_from  method", "stress test");

	std::vector<uint8_t> storage(storage_size, 0xFF);

	BitSeq_t bseq, bseq2;
	std::size_t i, pos1_1, pos1_2, pos2;
	std::string bits_strcopy_tmp, bits_strcopy1, bits_strcopy2;
	bool bit_next;

	// Populate stream with random bits
	bseq.init(storage.data(), storage.data()+storage.size());
	for (i=0; i < CHAR_BIT*storage.size(); ++i) {
		bit_next = rand()%2;
		bseq << bit_next;
	}

	// Run stress test
	std::cout << std::noboolalpha;
	
	std::size_t test_counter;
	for (test_counter = 0; test_counter < max_test_count; ++test_counter) {
		// Choose a random source range & destination
		pos1_1 = rand() % (CHAR_BIT*storage.size());
		pos1_2 = rand() % (CHAR_BIT*storage.size());
		if (pos1_1 > pos1_2)
			std::swap(pos1_1,pos1_2);
		else
			++pos1_2;
		pos2 = rand() % (CHAR_BIT*storage.size()+1 - (pos1_2-pos1_1));

		//std::cout << "Copy range source  = [" << pos1_1 << ", " << pos1_2 << ")"
		//		<< std::endl;
		//std::cout << "Copy position dest = [" << pos2 << ", "
		//		<< pos2 + pos1_2-pos1_1 << ")" << std::endl;
		
		// Directly check current bits
		bseq.init(storage.data(), storage.data()+storage.size());
		for (i=0; i < CHAR_BIT*storage.size(); ++i) {
			bseq >> bit_next;
			bits_strcopy1.push_back(bit_next ? '1':'0');
		}
		//std::cout << "Source bits          = " << bits_strcopy1 << std::endl;
		
		// Assign bits from stream 1 to 2
		bits_strcopy_tmp = bits_strcopy1;
		for (i=0; i < pos1_2-pos1_1; ++i) {
			bits_strcopy1[pos2+i] = bits_strcopy_tmp[pos1_1+i];
		}
		//std::cout << "Expected result bits = " << bits_strcopy1 << std::endl;
		
		bseq.init(storage.data(), storage.data()+storage.size());
		bseq.skip_next(pos1_1);
		bseq2.init(storage.data(), storage.data()+storage.size());
		bseq2.skip_next(pos2);
		bseq2.write_from(bseq, pos1_2-pos1_1);
		
		// Directly check resulting bitstream contents
		bseq.init(storage.data(), storage.data()+storage.size());
		for (i=0; i < CHAR_BIT*storage.size(); ++i) {
			bseq >> bit_next;
			bits_strcopy2.push_back(bit_next ? '1':'0');
		}

		// Check results
		if (bits_strcopy1 != bits_strcopy2) {
			throw std::logic_error(
					std::string("read/write bits do not match;\n")
					+ bits_strcopy1 + std::string("\n")
					+ bits_strcopy2 + std::string("\n")
					+ diffs(bits_strcopy1, bits_strcopy2)
				);
		}

		bits_strcopy1.clear();
		bits_strcopy2.clear();
		// Populate previous source range with random bits
		bseq.init(storage.data(), storage.data()+storage.size());
		bseq.skip_next(pos1_1);
		for (i=pos1_1; i < pos1_2; ++i) {
			bit_next = rand()%2;
			bseq << bit_next;
		}
	}
	std::cout << std::boolalpha << test_counter << " tests passed!" << std::endl;
	return;
 }





void test_runall(std::size_t storage_size, std::size_t test_counter) {
	
	// Single bit in/out
	BitSequence_test_inout_stress<BitSequence<ENDIAN, false> >(
		storage_size, test_counter);
	BitSequence_test_inout_stress<BitSequence<ENDIAN, true> >(
		storage_size, test_counter);

	// Streak read/write
	BitSequence_test_readwritestreak_stress<BitSequence<ENDIAN, false> >(
		storage_size, test_counter);
	BitSequence_test_readwritestreak_stress<BitSequence<ENDIAN, true> >(
		storage_size, test_counter);

	// Write bits from integer
	//BitSequence_test_writefromint_predefrange<BitSequence<ENDIAN, false> >();
	//BitSequence_test_writefromint_predefrange<BitSequence<ENDIAN, true> >();

	// Read/write bits from integer
	BitSequence_test_readwriteint_predefrange<BitSequence<ENDIAN, false>, uint8_t>(
		storage_size);
	BitSequence_test_readwriteint_predefrange<BitSequence<ENDIAN, true>, uint8_t>(
		storage_size);

	// Exchange between (possibly intersecting) streams
	BitSequence_test_writefrom_stress<BitSequence<ENDIAN, false> >(
		storage_size, test_counter);
	BitSequence_test_writefrom_stress<BitSequence<ENDIAN, true> >(
		storage_size, test_counter);

}

int main() {
	test_runall(5, 100000);
}
