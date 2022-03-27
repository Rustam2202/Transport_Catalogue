#pragma once

#include "json.h"
#include "transport_catalogue.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include <istream>
#include <fstream>
#include <string>

using namespace transport_catalogue;
using namespace json;

Bus CreateBus(Document& document) {

}

inline void ReadJSON(std::fstream& input) {
	transport_catalogue::TransportCatalogue catalogue;

	char c;
	input >> c;
	if (c != '{') {
		throw;
	}

	while (true) {
		json::Document request_type = json::Load(input);
		input.get(); // ':' eat
		json::Document request_data = json::Load(input);

		if (request_type.GetRoot().AsString() == "base_requests") {

			for (auto request : request_data.GetRoot().AsArray()) {
				if (request.AsMap().at("type").AsString() == "Bus") {
					transport_catalogue::Bus bus;
					bus.bus = request.AsMap().at("name").AsString();
					bus.is_ring = request.AsMap().at("is_roundtrip").AsBool();

					for (auto stop : request.AsMap().at("stops").AsArray()) {
						catalogue.AddStop();
					}
					//catalogue.AddBus();
				}
				else if (request.AsMap().at("type").AsString() == "Stop") {

				}
				else {
					throw;
				}
			}

			auto a = request_data.GetRoot().AsArray();
			auto b = a[0].AsMap();
			auto d = b.at("type");
			d.AsString() == "Bus";
		}
		else if (request_type.GetRoot().AsString() == "stat_requests") {

		}
		else {
			throw;
		}

		if (input.get() != ',') {
			break;
		}
	}



}

