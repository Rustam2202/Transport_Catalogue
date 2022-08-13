#pragma once

#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

void ReadJSON(std::istream& input = std::cin, std::ostream& output = std::cout);

pair<TransportCatalogue,string> MakeBase(std::istream& input);

void ProcessRequest(std::istream& input);