#include "json_reader.h"
#include "log_duration.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <fstream>
#include <string>

int main() {
	using namespace std;

	fstream input_file("test_12_3_input.json"s);
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

// s12_final_opentest_3