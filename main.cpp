#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main() {
	TransportCatalogue transport_catalogue;
	InputReader(transport_catalogue);
	OutputReader(transport_catalogue);
}