// чтение запросов на заполнение базы

#pragma once

#include "transport_catalogue.h"

#include <istream>
#include <string_view>

namespace transport_catalogue {
	void InputReader(TransportCatalogue& catalogue);
	//void InputReader(TransportCatalogue& catalogue,std::istream&);

}