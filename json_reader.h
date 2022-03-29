#pragma once

#include "json.h"
#include "transport_catalogue.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include <algorithm>
#include <fstream>
#include <map>;
#include <string>

using namespace transport_catalogue;
using namespace json;

inline Dict MakeDictStop(int request_id, const std::string stop_name, TransportCatalogue& catalogue) {
	auto stop_info = catalogue.GetStopInfo();
	auto stop_finded = catalogue.FindStop(stop_name);

	if (stop_finded == nullptr) {
		return {
			{"request_id", request_id},
			{"error_message", "not found"} };
	}
	else {
		auto buses_finded = stop_info[{stop_name, stop_finded}];
		Array buses;
		for (auto bus : buses_finded) {
			buses.push_back(bus);
		}
		return {
			{"buses", buses},
			{"request_id", request_id}
		};
	}
}

inline void ReadJSON(std::istream& input, TransportCatalogue& catalogue) {

	//transport_catalogue::TransportCatalogue catalogue;
	std::vector<std::pair<std::string, Dict>> road_distances;

	char c;
	input >> c;
	if (c != '{') {
		throw;
	}
	Document request_type = Load(input); // "base_requests":
	input.get(); // ':'
	Document base_request_data = Load(input);
	input.get(); // ','
	request_type = json::Load(input); // "stat_requests":
	input.get(); // ':'
	Document state_request_data = Load(input);

	// stops insert
	for (auto base_data : base_request_data.GetRoot().AsArray()) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Stop stop;
			stop.stop = base_data.AsMap().at("name").AsString();
			stop.coodinates.lat = base_data.AsMap().at("latitude").AsDouble();
			stop.coodinates.lng = base_data.AsMap().at("longitude").AsDouble();
			road_distances.push_back({ stop.stop, base_data.AsMap().at("road_distances").AsMap() });
			catalogue.AddStop(std::move(stop));
		}
	}

	// distances insert
	for (auto stop : road_distances) {
		for (auto dist : stop.second) {
			catalogue.SetDistanceBetweenStops(stop.first, dist.first, dist.second.AsInt());
		}
	}

	// buses insert
	for (auto base_data : base_request_data.GetRoot().AsArray()) {
		if (base_data.AsMap().at("type").AsString() == "Bus") {
			Bus bus;
			bus.bus = base_data.AsMap().at("name").AsString();
			bus.is_ring = base_data.AsMap().at("is_roundtrip").AsBool();
			for (auto stop : base_data.AsMap().at("stops").AsArray()) {
				bus.stops_unique.insert(catalogue.FindStop(stop.AsString()));
				bus.stops_vector.push_back(catalogue.FindStop(stop.AsString()));
			}

			catalogue.AddBus(std::move(bus));
		}
	}

	Array result;
	for (auto stat_data : state_request_data.GetRoot().AsArray()) {
		if (stat_data.AsMap().at("type").AsString() == "Stop") {
			catalogue.AddStopInfo(stat_data.AsMap().at("name").AsString());
			result.push_back(MakeDictStop(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString(), catalogue));
		}
		else if (stat_data.AsMap().at("type").AsString() == "Bus") {
			catalogue.GetBusInfo(stat_data.AsMap().at("name").AsString());
		}
	}
}

