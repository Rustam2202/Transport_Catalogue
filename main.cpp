
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
	
	if (argc != 2) {
		PrintUsage();
		return 1;
	}

	const std::string_view mode(argv[1]);
	fstream input_file("make_base_1.json"s);
	istream& strm(input_file);

	if (mode == "make_base"sv) {
		// make base here
		Serialization(strm);
	}
	else if (mode == "process_requests"sv) {

		// process requests here

	}
	else {
		PrintUsage();
		return 1;
	}
}