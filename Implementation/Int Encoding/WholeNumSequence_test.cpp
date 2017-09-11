// TODO remove after testing
//#define public private

#include "BitSequence.h"
#include "WholeNumSequence.h"
#include <iostream>
#include <vector>
#include <algorithm>

//using namespace BitSequence_NS;
static const std::size_t SIZE = 16;
static const bool ENDIAN = LITTLE;
static const std::vector<std::string> BITS_BY_NO({	// for little endian ONLY
	"0",		// 1
	"100",		// 2
	"1010",		// 3
	"1011",		// 4
	"11000",	// 5
	"11001",	// 6
	"110100",	// 7
	"110110",	// 8
	"110101",	// 9
	"110111",	// 10
	"1110000",	// 11
	"1110010",	// 12
	"1110001",	// 13
	"1110011",	// 14
	"11101000",	// 15
	"11101100",	// 16
	"11101010",	// 17
	"11101110",	// 18
	"11101001",	// 19
	"11101101",	// 20
	"11101011",	// 21
	"11101111"	// 22
});
typedef typename WholeNumSequence<ENDIAN>::wnum_t wnum_t;

/*******************************************************************************
 * TESTS - encoding_bitlength
 ******************************************************************************/
void WholeNumSequence_test_encoding_bitlength_predefrange() {
    std::cout << "Test Routine:\tWholeNumSequence class" << std::endl;
	std::cout << "\ttarget:\tencoding_bitlength static method" << std::endl;
	std::cout << "\ttype:\tpre-defined input range" << std::endl;

    typename WholeNumSequence<ENDIAN>::wnum_t n;
    std::size_t bitlength;

    std::cout << "Beginning test." << std::endl;
    for (n=1; true; ++n) {
        std::cout << "n = " << n << '\t';
        bitlength = WholeNumSequence<ENDIAN>::encoding_bitlength(n);
        if (bitlength <= CHAR_BIT*SIZE) {
            std::cout << "bit length = " << bitlength << '\t' << std::endl;
        } else {
            std::cout << "bit length exceeds storage size" << std::endl;
            break;
        }
    }
    std::cout << "Ending test." << std::endl;
}

/*******************************************************************************
* TESTS - Methods has_next, peek_next, skip_next
*******************************************************************************/

void WholeNumSequence_test_haspeekskipnext_stress() {
	std::cout << "Test Routine:\tWholeNumSequence class" << std::endl;
	std::cout << "\ttarget:\tmethods has_next, peek_next, skip_next" << std::endl;
	std::cout << "\ttype:\tstress test" << std::endl;

	std::cout << "Allocating " << SIZE << " bytes in memory... ";
	byte storage[SIZE];
	std::cout << "done." << std::endl;

    WholeNumSequence<ENDIAN> wseq;
	BitSequence<ENDIAN> bseq;
    std::vector<wnum_t> wnums_copy1, wnums_copy2;
    wnum_t n;
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
				bseq.set_next(c=='1');
			}
        }
        std::cout << "\ndone." << std::endl;
        // Retrieve all numbers from stream
        std::cout << "Reading output list from stream...";
        wseq.init(BitSequence<ENDIAN>(storage, storage+SIZE));
        while (wseq.has_next()) {
			wseq.peek_next(n);
			wseq.skip_next();
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
			BitSequence<ENDIAN> bseq(storage, storage+SIZE);
			while (bseq.has_next()) {
				std::cout << (bseq.get_next() ? '1':'0');
			}
			std::cout << std::endl;
        }
    }
    std::cout << "Exiting test." << std::endl;
}



/*******************************************************************************
* TESTS - Methods get_next, set_next
*******************************************************************************/

void WholeNumSequence_test_setnext_predefrange() {
	std::cout << "Test Routine:\tWholeNumSequence class" << std::endl;
	std::cout << "\ttarget:\tset_next method" << std::endl;
	std::cout << "\ttype:\tpre-defined input range" << std::endl;

	WholeNumSequence<ENDIAN> wseq;
	BitSequence<ENDIAN> bseq;
	std::size_t n, offset;
	byte storage[2]{255,255};
	std::string bits_copy;
	bool test_status = true;

	std::cout << "Begin Test." << std::endl;
	for (n = 1; test_status && n <= BITS_BY_NO.size(); ++n) {
		std::cout << "n = " << n << std::endl;
		for (offset = 0; test_status && offset < CHAR_BIT; ++offset) {
			std::cout << "Setting number in sequence at offset "
					<< offset << "... ";
			storage[0] = storage[1] = 255;
			bseq.init(storage, storage+2);
			bseq.skip_next(offset);
			wseq.init(bseq);
			if (!wseq.set_next(n)) {
				std::cout << "error: insufficient room to store no." << std::endl;
				test_status = false;
				break;
			}
			std::cout << "done." << std::endl;

			std::cout << "Comparing bits to expected value... ";
			bseq.init(storage, storage+2);
			bseq.skip_next(offset);
			bits_copy.clear();
			while (bseq.has_next()) {
				bits_copy.push_back(bseq.get_next() ? '1':'0');
			}

			if (BITS_BY_NO[n-1] != bits_copy.substr(
					0, BITS_BY_NO[n-1].size())
				) {
				std::cout << "Failed. Bits stored:" << std::endl
						<< bits_copy << std::endl;
				std::cout << "Bits expected:" << std::endl
						<< BITS_BY_NO[n-1] << std::endl;
				test_status = false;
				break;
			}
			std::cout << "Passed (stored = expected)" << std::endl;
		}
	}
	std::cout << "Test " << (test_status ? "passed!" : "failed.") << std::endl;
	std::cout << "Exiting test." << std::endl;
}

void WholeNumSequence_test_getnext_predefrange() {
	std::cout << "Test Routine:\tWholeNumSequence class" << std::endl;
	std::cout << "\ttarget:\tget_next method" << std::endl;
	std::cout << "\ttype:\tpre-defined input range" << std::endl;

	WholeNumSequence<ENDIAN> wseq;
	BitSequence<ENDIAN> bseq;
	std::size_t n, offset;
	typename WholeNumSequence<ENDIAN>::wnum_t n2;
	byte storage[2]{255,255};
	bool test_status = true;

	std::cout << "Begin Test." << std::endl;
	for (n = 1; test_status && n <= BITS_BY_NO.size(); ++n) {
		std::cout << "n = " << n << std::endl;
		for (offset = 0; test_status && offset < CHAR_BIT; ++offset) {
			std::cout << "Setting bits directly in sequence at offset "
					<< offset << "... ";
			storage[0] = storage[1] = 255;
			bseq.init(storage, storage+2);
			bseq.skip_next(offset);
			for (char c : BITS_BY_NO[n-1]) {
				bseq.set_next(c == '1');
			}
			std::cout << "done." << std::endl;

			std::cout << "Reading bits as number... ";
			bseq.init(storage, storage+2);
			bseq.skip_next(offset);
			wseq.init(bseq);
			if (!wseq.get_next(n2)) {
				std::cout << "error: failed to read no." << std::endl;
				test_status = false;
				break;
			}
			std::cout << "done." << std::endl;

			std::cout << "Comparing number to expected value... ";
			if (n != n2) {
				std::cout << "Failed.\n\tn2 = " << n2 << std::endl;
				test_status = false;
				break;
			}
			std::cout << "Passed (read = expected)" << std::endl;
		}
	}
	std::cout << "Test " << (test_status ? "passed!" : "failed.") << std::endl;
	std::cout << "Exiting test." << std::endl;
}


void WholeNumSequence_test_getsetnext_stress() {
    std::cout << "Test Routine:\tWholeNumSequence class" << std::endl;
 	std::cout << "\ttargets: operators <<, >>; has_next/fits_next methods"
            << std::endl;
 	std::cout << "\ttype:\tstress" << std::endl;

    std::cout << "Allocating " << SIZE << " bytes in memory... ";
	byte storage[SIZE];
	std::cout << "done." << std::endl;

    WholeNumSequence<ENDIAN> wseq;
    std::vector<wnum_t> wnums_copy1, wnums_copy2;
    wnum_t n;
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
        wseq.init(BitSequence<ENDIAN>(storage, storage+SIZE));
        while (n = 1 + (rand() % (1<<SIZE)), wseq.set_next(n)) {
            std::cout << '\t' << n << ',';
            wnums_copy1.push_back(n);
        }
        std::cout << "\ndone." << std::endl;
        // Retrieve all numbers from stream
        std::cout << "Reading output list from stream...";
        wseq.init(BitSequence<ENDIAN>(storage, storage+SIZE));
        while (wseq.get_next(n)) {
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
			BitSequence<ENDIAN> bseq(storage, storage+SIZE);
			while (bseq.has_next()) {
				std::cout << (bseq.get_next() ? '1':'0');
			}
			std::cout << std::endl;
        }
    }
    std::cout << "Exiting test." << std::endl;
}


int main() {
	//WholeNumSequence_test_encoding_bitlength_predefrange();
	//WholeNumSequence_test_haspeekskipnext_stress();
	//WholeNumSequence_test_setnext_predefrange();
	//WholeNumSequence_test_getnext_predefrange();
    //WholeNumSequence_test_getsetnext_stress();
}
