#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

#include <istream>
#include <string>

int main() {
	using namespace transport_catalogue;
	//using namespace std;
	TransportCatalogue transport_catalogue;
	//std::string input =  "13\n Stop Tolstopaltsevo : 55.611087, 37.20829, 3900m to Marushkino"s ;
	InputReader(transport_catalogue);
	//InputReader(transport_catalogue, input);
	OutputReader(transport_catalogue);
}