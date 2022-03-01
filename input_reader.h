// чтение запросов на заполнение базы

#pragma once

#include "transport_catalogue.h"

#include <string_view>

namespace transport_catalogue {
	void InputReader(TransportCatalogue& catalogue);
}