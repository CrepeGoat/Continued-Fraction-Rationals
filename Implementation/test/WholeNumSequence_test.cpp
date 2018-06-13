#include "WholeNumSeq_SBSBase.hpp"
#include "WholeNumSeq_SBS1.hpp"
#include "WholeNumSeq_SBS2.hpp"
#include "WholeNumSequence.hpp"

#include "BitSequence.hpp"
#include <iostream>
#include <exception>
#include <vector>
#include <algorithm>

static void cout_test_header(
		const std::string& str_title, 
		const std::string& str_target,
		const std::string& str_type) {
	std::cout << "Test Routine:\t" << str_title << std::endl;
	std::cout << "\ttarget:\t\t" << str_target << std::endl;
	std::cout << "\ttype:\t\t" << str_type << std::endl;
}

//using namespace BitSequence_NS;
static constexpr std::size_t SIZE = 64;

typedef BitSequence<false> BitSeq_t;
typedef WholeNumSequence WholeNumSeqt;

template <typename WNSeq_t>
std::string class_name();
template <>
std::string class_name<WholeNumSeqSBSBase>() {
	return "WholeNumSeq_SBSBase";
}
template <>
std::string class_name<WholeNumSeqSBS1>() {
	return "WholeNumSeq_SBS1";
}
template <>
std::string class_name<WholeNumSeqSBS2>() {
	return "WholeNumSeq_SBS2";
}
template <>
std::string class_name<WholeNumSequence>() {
	return "WholeNumSequence";
}

/*******************************************************************************
 * TESTS - encoding_bitlength
 ******************************************************************************/
/*
void WholeNumSequence_test_encoding_bitlength_predefrange() {
    std::cout << "Test Routine:\tWholeNumSequence class" << std::endl;
	std::cout << "\ttarget:\tencoding_bitlength static method" << std::endl;
	std::cout << "\ttype:\tpre-defined input range" << std::endl;

    uintmax_t n;
    std::size_t bitlength;

    std::cout << "Beginning test." << std::endl;
    for (n=1; true; ++n) {
        std::cout << "n = " << n << '\t';
        bitlength = WholeNumSeqt::encoding_bitlength(n);
        if (bitlength <= CHAR_BIT*SIZE) {
            std::cout << "bit length = " << bitlength << '\t' << std::endl;
        } else {
            std::cout << "bit length exceeds storage size" << std::endl;
            break;
        }
    }
    std::cout << "Ending test." << std::endl;
}
//*/
/*******************************************************************************
* TESTS - Methods has_next, peek_next, skip_next
*******************************************************************************/

/*
void WholeNumSequence_test_haspeekskipnext_stress() {
	std::cout << "Test Routine:\tWholeNumSequence class" << std::endl;
	std::cout << "\ttarget:\tmethods has_next, peek_next, skip_next" << std::endl;
	std::cout << "\ttype:\tstress test" << std::endl;

	std::cout << "Allocating " << SIZE << " bytes in memory... ";
	uint8_t storage[SIZE];
	std::cout << "done." << std::endl;

    WholeNumSeqt wnseq;
	BitSeq_t bseq;
    std::vector<uintmax_t> wnums_copy1, wnums_copy2;
    uintmax_t n;
    std::size_t i;
    bool test_status = true;

    std::cout << "Beginning test." << std::endl;
    while(test_status) {
		// Reset storage values
        std::fill(storage, storage+SIZE, -1);
		wnums_copy1.clear();
		wnums_copy2.clear();
        // Fill number stream with random numbers
		std::cout << "Generating/storing random inputs..." << std::endl;
        bseq.init(storage, storage+SIZE);
        while (n = 1 + (rand() % BITS_BY_NO.size()),
				bseq.has_next(BITS_BY_NO[n-1].size())
			) {
            std::cout << '\t' << n << ',';
            wnums_copy1.push_back(n);
			for (char c : BITS_BY_NO[n-1]) {
				bseq.write_next(c=='1');
			}
        }
        std::cout << "\ndone." << std::endl;
        // Retrieve all numbers from stream
        std::cout << "Reading output list from stream...";
        wnseq.init(BitSeq_t(storage, storage+SIZE));
        while (wnseq.has_next()) {
			wnseq.peek_next(n);
			wnseq.skip_next();
			wnums_copy2.push_back(n);
        }
        std::cout << "done." << std::endl;
        // Compare list copies
        std::cout << "Comparing input/output lists... ";
        test_status = (wnums_copy1 == wnums_copy2);
        if (test_status) {
            std::cout << "Success! (input = output)" << std::endl;
        } else {
            std::cout << "Failure! Output list:" << std::endl;
            for (i = 0; i < std::min(wnums_copy1.size(), wnums_copy2.size()); ++i) {
                std::cout << '\t' << (wnums_copy1[i] != wnums_copy2[i] ? "*":"")
                        << wnums_copy2[i] << ',';
            }
            for (; i < wnums_copy2.size(); ++i) {
                std::cout << "\t*" << wnums_copy2[i] << ',';
            }
            std::cout << std::endl;
			std::cout << "Bits Stored: ";
			BitSeq_t bseq(storage, storage+SIZE);
			while (bseq.has_next()) {
				std::cout << (bseq.read_next() ? '1':'0');
			}
			std::cout << std::endl;
        }
    }
    std::cout << "Exiting test." << std::endl;
}
//*/



/*******************************************************************************
* READ/WRITE UTILITIES
*******************************************************************************/
// Test/stubbing constructs
static const std::vector<std::string> SBSBASE_TABLE({
	// for msb-first sequences
	"00",		// 3
	"010",		// 4
	"011",		// 5
	"1000",		// 6
	"1001",		// 7
	"10100",	// 8
	"10101",	// 9
	"10110",	// 10
	"10111",	// 11
	"110000",	// 12
	"110001",	// 13
	"110010",	// 14
	"110011",	// 15
	"1101000",	// 16
	"1101001",	// 17
	"1101010",	// 18
	"1101011",	// 19
	"1101100",	// 20
	"1101101",	// 21
	"1101110",	// 22
	"1101111"	// 23
});

template <typename WNSeq_t>
uintmax_t first_symbol() {
	return 1;
}
template <>
uintmax_t first_symbol<WholeNumSeqSBSBase>() {
	return 3;
}

template <typename WNSeq_t>
bool has_static_encoding(uintmax_t val);
template <>
bool has_static_encoding<WholeNumSeqSBSBase>(uintmax_t val) {
	return val-3 < SBSBASE_TABLE.size();
}
template <>
bool has_static_encoding<WholeNumSeqSBS1>(uintmax_t val) {
	return val-1 < SBSBASE_TABLE.size() + 1;
}
template <>
bool has_static_encoding<WholeNumSeqSBS2>(uintmax_t val) {
	return val-1 < SBSBASE_TABLE.size() + 2;
}

bool has_static_encoding(uintmax_t val, bool ENC_SBS1) {
	return val-1 < SBSBASE_TABLE.size() + 1 + !ENC_SBS1;
}





template <typename WNSeq_t>
std::string encoding_str(uintmax_t val);
template <>
std::string encoding_str<WholeNumSeqSBSBase>(uintmax_t val) {
	return SBSBASE_TABLE[val-3];
}
template <>
std::string encoding_str<WholeNumSeqSBS1>(uintmax_t val) {
	return (val == 1) ? "0" : (std::string("1") + SBSBASE_TABLE[val-2]);
}
template <>
std::string encoding_str<WholeNumSeqSBS2>(uintmax_t val) {
	return (val == 1) ? "00"
		: ((val == 2) ? "01"
		: (std::string("1") + SBSBASE_TABLE[val-3]));
}

std::string encoding_str(uintmax_t val, bool ENC_SBS1) {
	//if (!has_static_encoding(val,ENC_SBS1))
	//	return "";
	
	if (ENC_SBS1 && val <= 1)
		return "0";
	if (!ENC_SBS1 && val <= 2)
		return (val == 1) ? "00" : "01";

	return std::string("1") + SBSBASE_TABLE.at(val + ENC_SBS1 - 3);
}




class RhoTracker {
public:
	std::pair<uintmax_t, uintmax_t> rho_frac{0,1};

	inline void push_back(uintmax_t next_val) {
		//rho = 1 / (rho + next_val);
		rho_frac = {
				rho_frac.second,
				rho_frac.first + rho_frac.second * next_val
			};
	}
	inline bool should_use_SBS1() {
		//return (rho < .75);
		return rho_frac.first * 4 < rho_frac.second * 3;
	}
};


/*******************************************************************************
* READ/WRITE FUNCTIONS
*******************************************************************************/

bool write_bits_from_str(
		BitSeq_t& bit_seq,
		const std::string& bits_str
	)
{
	if (!bit_seq.has_next(bits_str.length()))
		return false;

	for (auto iter = bits_str.begin(); iter < bits_str.end(); ++iter) {
		bit_seq << (*iter == '1');
	}
	return true;
}
bool read_bits_to_str(
		BitSeq_t& bit_seq,
		std::string& bits_str,
		std::size_t length
	)
{
	if (!bit_seq.has_next(length))
		return false;

	for (std::size_t i = 0; i < length; ++i) {
		bits_str.append(bit_seq.read_next() ? "1" : "0");
	}
	return true;
}





template <typename WNSeq_t>
bool write_bits_from_seq(
		BitSeq_t& bit_seq,
		const std::vector<uintmax_t>& seq
	)
{
	for (auto iter = seq.begin(); iter < seq.end(); ++iter) {
		if (
				!has_static_encoding<WNSeq_t>(*iter)
				|| !write_bits_from_str(bit_seq, encoding_str<WNSeq_t>(*iter))
			)
			return false;
	}
	return true;
}

template <typename WNSeq_t>
bool check_nums_against_wnumseq(
		WNSeq_t& wnseq, 
		const std::vector<uintmax_t>& seq
	)
{
	uintmax_t num;
	for (auto iter_nums = seq.begin(); iter_nums < seq.end(); ++iter_nums) {
		if (!wnseq.read_next(num) || *iter_nums != num)
			return false;
	}
	return true;
}



template <typename WNSeq_t>
bool write_nums_to_wnumseq(
		WNSeq_t& wnseq,
		const std::vector<uintmax_t>& seq
	)
{
	for (auto iter = seq.begin(); iter < seq.end(); ++iter) {
		if (!has_static_encoding<WNSeq_t>(*iter) || !wnseq.write_next(*iter))
			return false;
	}
	return true;
}

template <typename WNSeq_t>
bool check_bits_against_seq(
		BitSeq_t& bit_seq,
		const std::vector<uintmax_t>& seq
	)
{
	std::string bits_str, bits_str_expected;

	for (auto iter = seq.begin(); iter < seq.end(); ++iter) {
		bits_str.clear();
		bits_str_expected = encoding_str<WNSeq_t>(*iter);
		if (!read_bits_to_str(bit_seq, bits_str, bits_str_expected.length())
				|| bits_str != bits_str_expected)
			return false;
	}
	return true;
}







template <>
bool write_bits_from_seq<WholeNumSequence>(
		BitSeq_t& bit_seq,
		const std::vector<uintmax_t>& seq
	)
{
	RhoTracker tracker;
	for (auto iter = seq.begin(); iter < seq.end(); ++iter) {
		if (
				!has_static_encoding(*iter, tracker.should_use_SBS1())
				|| !write_bits_from_str(bit_seq, encoding_str(*iter, tracker.should_use_SBS1()))
			)
			return false;
		tracker.push_back(*iter);
	}
	return true;
}


template <>
bool write_nums_to_wnumseq<WholeNumSequence>(
	WholeNumSequence& wnseq, const std::vector<uintmax_t>& seq)
{
	RhoTracker tracker;
	for (auto iter = seq.begin(); iter < seq.end(); ++iter) {
		if (
				!has_static_encoding(*iter, tracker.should_use_SBS1())
				|| !wnseq.write_next(*iter)
			)
			return false;
		tracker.push_back(*iter);
	}
	return true;
}
template <>
bool check_bits_against_seq<WholeNumSequence>(
	BitSeq_t& bit_seq, const std::vector<uintmax_t>& seq)
{
	RhoTracker tracker;
	std::string bits_str, bits_str_expected;

	for (auto iter = seq.begin(); iter < seq.end(); ++iter) {
		bits_str.clear();
		bits_str_expected = encoding_str(*iter, tracker.should_use_SBS1());
		if (
				!read_bits_to_str(bit_seq, bits_str, bits_str_expected.length())
				|| bits_str != bits_str_expected
			) 
			return false;
		tracker.push_back(*iter);
	}
	return true;
}




/*******************************************************************************
* TESTS - read_next
*******************************************************************************/

template <typename WNSeq_t>
bool test_WholeNumSeq_readnext(
		const std::vector<uintmax_t>& vals,
		std::size_t storage_size = 1
	)
{
	static std::vector<uint8_t> storage;
	storage.assign(storage_size, 0xFF);

	BitSeq_t bseq(storage.data(), storage.data()+storage.size());
	if (!write_bits_from_seq<WNSeq_t>(bseq, vals)) {
		return false;
	}

	WNSeq_t wnseq(BitSeq_t(storage.data(), storage.data()+storage.size()));
	if (!check_nums_against_wnumseq(wnseq, vals)) {
		throw std::logic_error(
			"read values do not match original sequence");
	}

	return true;
}

template <typename WNSeq_t>
void WholeNumSequence_test_readnext_predefrange(std::size_t storage_size) {
	cout_test_header(
			class_name<WNSeq_t>() + " class",
			"read_next method",
			"pre-defined input range"
		);

	std::size_t test_case_counter = 0;

	std::vector<uintmax_t> v_nums;
	std::vector<uintmax_t>::iterator iter_nums;

	while(true) {
		v_nums.push_back(first_symbol<WNSeq_t>());
		
		while (!test_WholeNumSeq_readnext<WNSeq_t>(v_nums, storage_size)) {
			v_nums.pop_back();
			if (v_nums.empty()) {	
				std::cout << test_case_counter << " test cases passed!\n"
					<< std::endl;
				return;
			}
			++(v_nums.back());
		}
		++test_case_counter;
	}
}


/*******************************************************************************
* TESTS - write_next
*******************************************************************************/

template <typename WNSeq_t>
bool test_WholeNumSeq_writenext(
		const std::vector<uintmax_t>& vals,
		std::size_t storage_size = 1
	)
{
	static std::vector<uint8_t> storage;
	storage.assign(storage_size, 0xFF);

	WNSeq_t wnseq(BitSeq_t(storage.data(), storage.data() + storage.size()));
	if (!write_nums_to_wnumseq<WNSeq_t>(wnseq, vals)){
		return false;
	}

	BitSeq_t bseq(storage.data(), storage.data()+storage.size());
	if (!check_bits_against_seq<WNSeq_t>(bseq, vals)) {
		throw std::logic_error(
			"written values do not match original sequence");
	}

	return true;
}

template <typename WNSeq_t>
void WholeNumSequence_test_writenext_predefrange(std::size_t storage_size) {
	cout_test_header(
			class_name<WNSeq_t>() + " class",
			"write_next method",
			"pre-defined input range"
		);

	std::size_t test_case_counter = 0;

	std::vector<uintmax_t> v_nums;
	std::vector<uintmax_t>::iterator iter_nums;

	while(true) {
		v_nums.push_back(first_symbol<WNSeq_t>());
		
		while (!test_WholeNumSeq_writenext<WNSeq_t>(v_nums, storage_size)) {
			v_nums.pop_back();
			if (v_nums.empty()) {	
				std::cout << test_case_counter << " test cases passed!\n" << std::endl;
				return;
			}
			++(v_nums.back());
		}
		++test_case_counter;
	}
}




template <typename WNSeq_t>
void WholeNumSequence_test_readwritenext_stress() {
    std::cout << "Test Routine:\tWholeNumSequence class" << std::endl;
 	std::cout << "\ttargets: operators <<, >>; has_next/fits_next methods"
            << std::endl;
 	std::cout << "\ttype:\tstress" << std::endl;

    std::cout << "Allocating " << SIZE << " bytes in memory... ";
	uint8_t storage[SIZE];
	std::cout << "done." << std::endl;

    WNSeq_t wnseq;
    std::vector<uintmax_t> wnums_copy1, wnums_copy2;
    uintmax_t n;
    std::size_t i;
    bool test_status = true;

    std::cout << "Beginning test." << std::endl;
    while(test_status) {
		// Reset storage values
        std::fill(storage, storage+SIZE, -1);
		wnums_copy1.clear();
		wnums_copy2.clear();
        // Fill number stream with random numbers
		std::cout << "Generating/storing random inputs..." << std::endl;
        wnseq.init(BitSeq_t(storage, storage+SIZE));
        while (n = 1 + (rand() % (uintmax_t(1)<<std::min(SIZE, std::size_t(3)))),//CHAR_BIT*sizeof(uintmax_t)-1))),
				wnseq.write_next(n)) {
            std::cout << '\t' << n << ',';
            wnums_copy1.push_back(n);
        }
        std::cout << "\ndone." << std::endl;
        // Retrieve all numbers from stream
        std::cout << "Reading output list from stream...";
        wnseq.init(BitSeq_t(storage, storage+SIZE));
        while (wnseq.read_next(n)) {
            wnums_copy2.push_back(n);
        }
        std::cout << "done." << std::endl;
        // Compare list copies
        std::cout << "Comparing input/output lists... ";
        test_status = (wnums_copy1 == wnums_copy2);
        if (test_status) {
            std::cout << "Success! (input = output)" << std::endl;
        } else {
            std::cout << "Failure! Output list:" << std::endl;
            for (i = 0; i < std::min(wnums_copy1.size(), wnums_copy2.size()); ++i) {
                std::cout << '\t' << (wnums_copy1[i] != wnums_copy2[i] ? "*":"")
                        << wnums_copy2[i] << ',';
            }
            for (; i < wnums_copy2.size(); ++i) {
                std::cout << "\t*" << wnums_copy2[i] << ',';
            }
            std::cout << std::endl;
			std::cout << "Bits Stored: ";
			BitSeq_t bseq(storage, storage+SIZE);
			while (bseq.has_next()) {
				std::cout << (bseq.read_next() ? '1':'0');
			}
			std::cout << std::endl;
        }
    }
    std::cout << "Exiting test." << std::endl;
}


void test_runall() {
	std::cout << "==============================================\n";
	WholeNumSequence_test_readnext_predefrange<WholeNumSeqSBSBase>(2);
	WholeNumSequence_test_writenext_predefrange<WholeNumSeqSBSBase>(2);

	std::cout << "==============================================\n";
	WholeNumSequence_test_readnext_predefrange<WholeNumSeqSBS1>(2);
	WholeNumSequence_test_writenext_predefrange<WholeNumSeqSBS1>(2);

	std::cout << "==============================================\n";
	WholeNumSequence_test_readnext_predefrange<WholeNumSeqSBS2>(2);
	WholeNumSequence_test_writenext_predefrange<WholeNumSeqSBS2>(2);

	std::cout << "==============================================\n";
	WholeNumSequence_test_readnext_predefrange<WholeNumSequence>(2);
	WholeNumSequence_test_writenext_predefrange<WholeNumSequence>(2);
	
	std::cout << "==============================================" << std::endl;
}

int main() {
	test_runall();
}
