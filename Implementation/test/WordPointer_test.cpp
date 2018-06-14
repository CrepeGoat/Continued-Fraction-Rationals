#include "WordPointer.hpp"

#include "endian.hpp"
#include <exception>


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




// TODO implement tests for WordPointer class
// 	- include tests for both forward/reverse byte orderings
//	- include tests for dereferencing of all word sizes





void test_runall() {
	// TODO
}

int main() {
	test_runall();
}
