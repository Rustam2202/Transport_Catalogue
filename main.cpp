#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

#include <iostream>
#include <string>

int main() {
	using namespace transport_catalogue;
	TransportCatalogue transport_catalogue;
	InputReader(transport_catalogue);
	OutputReader(transport_catalogue);
}