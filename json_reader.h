#pragma once

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "json.h"
#include "log_duration.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

using namespace transport_catalogue;
using namespace json;
using namespace std::literals;

inline void ReadJSON(std::istream& input = std::cin, std::ostream& output = std::cout) {
	Node base = Load(input).GetRoot();
	transport_catalogue::TransportCatalogue catalogue;
	renderer::MapRenderer mr;
	RequestHandler rh(catalogue, mr);

	rh.InsertStops(base.AsMap().at("base_requests").AsArray());

	rh.InsertStopsDistances(base.AsMap().at("base_requests").AsArray());

	rh.InsertBuses(base.AsMap().at("base_requests").AsArray());

	Array result;
	rh.CompileStats(base.AsMap().at("stat_requests").AsArray(), result);
	Document doc(std::move(result));
	Print(doc, output);
}


