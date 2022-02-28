// чтение запросов на заполнение базы

#pragma once

#include "transport_catalogue.h"

#include <string_view>

namespace TransportCatalogueNamespace {
	void InputReader(TransportCatalogue& trans_cat);
}