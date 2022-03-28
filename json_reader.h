#pragma once

#include "json.h"
#include "transport_catalogue.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include <fstream>
#include <map>;
#include <string>

using namespace transport_catalogue;
using namespace json;

inline TransportCatalogue ReadJSON(std::fstream& input) {
	
	transport_catalogue::TransportCatalogue catalogue;
	std::vector<std::pair<std::string, Dict>> road_distances;

	char c;
	input >> c;
	if (c != '{') {
		throw;
	}
	Document request_type = Load(input); // "base_requests":
	input.get(); // ':'
	Document base_request_data = Load(input); // 
	input.get(); // ','
	request_type = json::Load(input); // "stat_requests":
	input.get(); // ':'
	Document state_request_data = Load(input);

	for (auto base_data : base_request_data.GetRoot().AsArray()) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Stop stop;
			stop.stop = base_data.AsMap().at("name").AsString();
			stop.coodinates.lat = base_data.AsMap().at("latitude").AsDouble();
			stop.coodinates.lng = base_data.AsMap().at("longitude").AsDouble();
			road_distances.push_back({stop.stop, base_data.AsMap().at("road_distances").AsMap() });
			catalogue.AddStop(std::move(stop));
		}
	}

	for (auto stop : road_distances) {
		for (auto dist : stop.second) {
			catalogue.SetDistanceBetweenStops(stop.first, dist.first, dist.second.AsInt());
		}
	}

	for (auto base_data : base_request_data.GetRoot().AsArray()) {
		if (base_data.AsMap().at("type").AsString() == "Bus") {
			Bus bus;
			bus.bus= base_data.AsMap().at("name").AsString();
			bus.is_ring= base_data.AsMap().at("is_roundtrip").AsBool();
			for (auto stop : base_data.AsMap().at("stops").AsArray()) {
				bus.stops_vector.push_back(catalogue.FindStop(stop.AsString()));
			}
			catalogue.AddBus(std::move(bus));
		}
	}

	return catalogue;
}

