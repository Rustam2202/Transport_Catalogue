#pragma once

#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

void InsertStops(TransportCatalogue& catalogue, Array base);

void InsertStopsDistances(TransportCatalogue& catalogue, Array base);

void InsertBuses(TransportCatalogue& catalogue, Array base);

void SetMapRender(MapRenderer& map, Node render_settings);

void ReadJSON(std::istream& input = std::cin, std::ostream& output = std::cout);

RequestHandler MakeBase(std::string& file_name,std::istream& input);

void ProcessRequest(std::istream& input);