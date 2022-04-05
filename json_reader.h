#pragma once

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "json.h"
#include "log_duration.h"
#include "transport_catalogue.h"

#include <algorithm>
#include <execution>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

using namespace transport_catalogue;
using namespace json;
using namespace std::literals;

inline Dict MakeDictStop(int request_id, std::string_view stop_name, TransportCatalogue& catalogue) {
	StopInfoType stop_info = catalogue.GetStopInfo();
	Stop* stop_finded = catalogue.FindStop(stop_name);

	if (stop_finded == nullptr) {
		return { {"request_id"s, request_id}, {"error_message"s, "not found"s} };
	}
	else {
		//std::set<Bus*> buses_finded = stop_info[{stop_name, stop_finded}];
		Array buses;

		/*for (Bus* bus : stop_info[{stop_name, stop_finded}]) {
			if (bus != nullptr) {
				buses.push_back((std::string)bus->bus);
			}
		}*/

		for (const std::string& bus : stop_info[{stop_name, stop_finded}]) {
			if (bus != nullptr) {
				buses.push_back(bus);
			}
		}
		return {
			{"buses"s, buses},
			{"request_id"s, request_id}
		};
	}
}

inline Dict MakeDictBus(int request_id, std::string_view bus_name, TransportCatalogue& catalogue) {
	auto bus_info = catalogue.GetBusInfo();
	auto bus_finded = catalogue.FindBus(bus_name);

	if (bus_finded == nullptr) {
		return { {"request_id"s, request_id}, {"error_message"s, "not found"s} };
	}
	else {
		BusInfo bus_finded = bus_info[bus_name];
		return{
			{"curvature"s, bus_finded.curvature},
			{"request_id"s, request_id},
			{ "route_length"s, (double)bus_finded.route_length_on_road},
			{"stop_count"s, (int)bus_finded.stops_on_route},
			{"unique_stop_count"s, (int)bus_finded.unique_stops}
		};
	}
}

inline void ReadJSON(TransportCatalogue& catalogue, std::istream& input = std::cin, std::ostream& output = std::cout) {

	//std::vector<std::pair<std::string, Dict>> road_distances;
	std::unordered_map<std::pair<std::string, std::string>, int, Hasher> road_distances_2;
	//	std::unordered_map<std::pair<std::string_view, std::string_view>, int, Hasher> road_distances_2;


	char c;
	input >> c;
	if (c != '{') {
		throw;
	}

	Document request_type = Load(input); // "base_requests":
	input.get(c); // ':'
	Document base_request_data = Load(input);
	input.get(); // ','
	request_type = json::Load(input); // "stat_requests":
	input.get(); // ':'
	Document state_request_data = Load(input);

	// stops insert
	std::vector<Dict> buses_dicts;
	for (Node base_data : base_request_data.GetRoot().AsArray()) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Stop stop;
			stop.stop = std::move(base_data.AsMap().at("name").AsString());
			stop.coodinates.lat = base_data.AsMap().at("latitude").AsDouble();
			stop.coodinates.lng = base_data.AsMap().at("longitude").AsDouble();
			//road_distances.push_back({ stop.stop, base_data.AsMap().at("road_distances").AsMap() });
			for (auto dict : base_data.AsMap().at("road_distances").AsMap()) {
				road_distances_2[{stop.stop, dict.first}] = std::move(dict.second.AsInt());
			}
			catalogue.AddStop(std::move(stop));
		}
		else if (base_data.AsMap().at("type").AsString() == "Bus") {
			buses_dicts.push_back(base_data.AsMap());
		}
	}


	// distances insert
	for (auto stop : road_distances_2) {
		catalogue.SetDistanceBetweenStops(stop.first.first, stop.first.second, stop.second);
	}

	/*for (std::pair<std::string, json::Dict> stop : road_distances) {
		for (auto dist : stop.second) {
			catalogue.SetDistanceBetweenStops(stop.first, dist.first, dist.second.AsInt());
		}
	}*/


	// buses insert
	for (Dict bus_dict : buses_dicts) {
		Bus bus;
		bus.bus = std::move(bus_dict.at("name").AsString());
		bus.is_ring = bus_dict.at("is_roundtrip").AsBool();
		for (Node stop : bus_dict.at("stops").AsArray()) {
			bus.stops_unique.insert(catalogue.FindStop(stop.AsString()));
			bus.stops_vector.push_back(catalogue.FindStop(stop.AsString()));
		}
		catalogue.AddBus(std::move(bus));
	}

	//for (Node base_data : base_request_data.GetRoot().AsArray()) {
	//	if (base_data.AsMap().at("type").AsString() == "Bus") {
	//		Bus bus;
	//		bus.bus = base_data.AsMap().at("name").AsString();
	//		bus.is_ring = base_data.AsMap().at("is_roundtrip").AsBool();
	//		for (Node stop : base_data.AsMap().at("stops").AsArray()) {
	//			bus.stops_unique.insert(catalogue.FindStop(stop.AsString()));
	//			bus.stops_vector.push_back(catalogue.FindStop(stop.AsString()));
	//		}
	//		catalogue.AddBus(std::move(bus));
	//	}
	//}


	// stat result

	Array result;
	for (auto stat_data : state_request_data.GetRoot().AsArray()) {
		if (stat_data.AsMap().at("type").AsString() == "Stop") {
			catalogue.AddStopInfo(stat_data.AsMap().at("name").AsString());
			result.push_back(MakeDictStop(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString(), catalogue));
		}
		else if (stat_data.AsMap().at("type").AsString() == "Bus") {
			catalogue.AddBusInfo(stat_data.AsMap().at("name").AsString());
			result.push_back(MakeDictBus(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString(), catalogue));
		}
	}
	Document doc(std::move(result));
	Print(doc, output);

}

