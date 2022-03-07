// чтение запросов на вывод и сам вывод

#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transport_catalogue {
	//void OutputReader(TransportCatalogue& catalogue);
	void OutputReader(TransportCatalogue& catalogue, std::istream& input = std::cin, std::ostream& output = std::cout);
}