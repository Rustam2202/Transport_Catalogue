#pragma once

#include "json.h"
#include "log_duration.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

void ReadJSON(std::istream& input = std::cin, std::ostream& output = std::cout);

TransportCatalogue MakeBase(std::istream& input);