#include "json_reader.h"
#include "log_duration.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <fstream>
#include <string>

int main() {
	using namespace std;

	fstream input_file("s12_final_opentest_3.json"s); // s10_final_opentest_1
	fstream output_file;
	istream& strm(input_file);
	output_file.open("test_12_1_output.json");
	{
		LOG_DURATION("speed");
		ReadJSON(strm, output_file); 
	}
	output_file.close();
}

//ReadJSON(std::cin, std::cout);
