
#include "BitTwiddles.h"
#include <iostream>


void test_msb(unsigned char max_bits = 64) {
	std::cout << "Test Routine - msb" << std::endl;
	bool test_status = true;

	std::cout << "Runnng on values [0,2^64)..." << std::endl;
	std::cout << "msb pos = -1, \tmsb = 0" << std::endl;
	if (msb(0ull) != 0) {
		std::cout << "FAILURE: val = 0; expected msb = 0, "
				<< "actual = " << msb(0ull) << std::endl;
		test_status = false;
	}
	std::cout << "msb pos = 0, \tmsb = 1" << std::endl;

	for (unsigned long long val = 1, i = 0; i < max_bits;) {
		if (msb(val) != (1ull<<i)) {
			std::cout << "FAILURE: val = " << val
					<< "; expected msb = " << (1ull<<i)
					<< ", actual = " << msb(val) << std::endl;
			test_status = false;
		}

		++val;
		if (!(val & (val-1))) {
			++i;
			std::cout << "msb pos = " << i << ", \tmsb = " << (1ull<<i)
					<< std::endl;
		}
	}

	std::cout << "Done. Test " << (test_status ?"Passed!" :"Failed!") << std::endl;
	std::cout << "Exiting test...\n" << std::endl;
}


static const unsigned long long values_by_msb_pos[65] = {
		1ull, 2ull, 4ull, 8ull, 16ull, 32ull, 64ull, 128ull, 256ull, 512ull,
		1024ull, 2048ull, 4096ull, 8192ull, 16384ull, 32768ull, 65536ull,
		131072ull, 262144ull, 524288ull, 1048576ull, 2097152ull, 4194304ull,
		8388608ull, 16777216ull, 33554432ull, 67108864ull, 134217728ull,
		268435456ull, 536870912ull, 1073741824ull, 2147483648ull, 4294967296ull,
		8589934592ull, 17179869184ull, 34359738368ull, 68719476736ull,
		137438953472ull, 274877906944ull, 549755813888ull, 1099511627776ull,
		2199023255552ull, 4398046511104ull, 8796093022208ull, 17592186044416ull,
		35184372088832ull, 70368744177664ull, 140737488355328ull,
		281474976710656ull, 562949953421312ull, 1125899906842624ull,
		2251799813685248ull, 4503599627370496ull, 9007199254740992ull,
		18014398509481984ull, 36028797018963968ull, 72057594037927936ull,
		144115188075855872ull, 288230376151711744ull, 576460752303423488ull,
		1152921504606846976ull, 2305843009213693952ull, 4611686018427387904ull, 
		9223372036854775808ull, 0ull
	};
void test_bit_pos_0l() {
	std::cout << "Test Routine - bit_pos_0l" << std::endl;
	bool test_status = true;

	std::cout << "Runnng on values {0} and {2^i for i in [0,64)}..." << std::endl;
	if (bit_pos_0l(0) != -1) {
		std::cout << "FAILURE: val = 0; expected msb position = -1, "
				<< "actual = " << bit_pos_0h(0) << std::endl;
		test_status = false;
	}
	for (int i=0; i<64; ++i) {
		if (bit_pos_0l(values_by_msb_pos[i]) != i) {
			std::cout << "FAILURE: val = " << values_by_msb_pos[i]
					<< "; expected msb position = " << i << ", "
					<< "actual = " << bit_pos_0h(values_by_msb_pos[i]) << std::endl;
			test_status = false;
		}
	}

	std::cout << "Done. Test " << (test_status ?"Passed!" :"Failed!") << std::endl;
	std::cout << "Exiting test...\n" << std::endl;
}


void test_bit_pos_0h() {
	std::cout << "Test Routine - bit_pos_0h" << std::endl;
	bool test_status = true;

	std::cout << "Runnng on values {2^i for i in [0,64)} and {0}..." << std::endl;
	for (int i=0; i<=64; ++i) {
		if (bit_pos_0h(values_by_msb_pos[i]) != i) {
			std::cout << "FAILURE: val = " << values_by_msb_pos[i]
					<< "; expected msb position = " << i << ", "
					<< "actual = " << bit_pos_0h(values_by_msb_pos[i]) << std::endl;
			test_status = false;
		}
	}

	std::cout << "Done. Test " << (test_status ?"Passed!" :"Failed!") << std::endl;
	std::cout << "Exiting test...\n" << std::endl;
}

int main() {
	//test_msb(32);
	//test_bit_pos_0l();
	test_bit_pos_0h();
	return 0;
}
