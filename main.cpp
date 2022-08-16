
#include "serialization.h"

#include <fstream>
#include <iostream>
#include <string_view>

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
	stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
	using namespace std;

	ifstream base_input("make_base_4.json"s); // s14_1_opentest_1_make_base   make_base_3
	Serialization(base_input);

	ifstream request_input("process_requests_4.json"s); // s14_1_opentest_1_process_requests   process_requests_3
	DeSerialization(request_input, std::cout);

	//if (argc != 2) {
	//	PrintUsage();
	//	return 1;
	//}
	//const std::string_view mode(argv[1]);
	//fstream input_file("make_base_1.json"s);
	//istream& strm(input_file);
	//if (mode == "make_base"sv) {
	//	// make base here
	//	Serialization(strm);
	//}
	//else if (mode == "process_requests"sv) {
	//	// process requests here
	//	DeSerialization(strm);
	//}
	//else {
	//	PrintUsage();
	//	return 1;
	//}
}