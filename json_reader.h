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
		Array buses_arr;
		if (stop_info.count(stop_finded)) {
			std::unordered_set<Bus*> buses = stop_info.at(stop_finded);
			for (Bus* bus : buses) {
				buses_arr.push_back(bus->bus_name);
			}
			std::sort(std::execution::par, buses_arr.begin(), buses_arr.end(), [](const Node& lhs, const Node& rhs) {return lhs.AsString() < rhs.AsString(); });
		}
		return {
			{"buses"s, buses_arr},
			{"request_id"s, request_id}
		};
	}
}

inline Dict MakeDictBus(int request_id, std::string_view bus_name, TransportCatalogue& catalogue) {
	BusInfoType bus_info = catalogue.GetBusInfo();
	Bus* bus_finded = catalogue.FindBus(bus_name);

	if (bus_finded == nullptr) {
		return { {"request_id"s, request_id}, {"error_message"s, "not found"s} };
	}
	else {
		BusInfo bus_info_finded = bus_info[bus_name];
		return{
			{"curvature"s, bus_info_finded.curvature},
			{"request_id"s, request_id},
			{ "route_length"s, (double)bus_info_finded.route_length_on_road},
			{"stop_count"s, (int)bus_info_finded.stops_on_route},
			{"unique_stop_count"s, (int)bus_info_finded.unique_stops}
		};
	}
}

inline void ReadJSON(TransportCatalogue& catalogue, std::istream& input = std::cin, std::ostream& output = std::cout) {
	Node base = Load(input).GetRoot();

	// stops insert
	for (Node base_data : base.AsMap().at("base_requests").AsArray()) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Stop stop;
			stop.stop_name = base_data.AsMap().at("name").AsString();
			stop.coodinates.lat = base_data.AsMap().at("latitude").AsDouble();
			stop.coodinates.lng = base_data.AsMap().at("longitude").AsDouble();
			catalogue.AddStop(std::move(stop));
		}
	}

	// stop-distances insert
	for (Node base_data : base.AsMap().at("base_requests").AsArray()) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Dict stops = base_data.AsMap().at("road_distances").AsMap();
			for (auto other_stop : stops) {
				catalogue.SetDistanceBetweenStops(base_data.AsMap().at("name").AsString(), other_stop.first, other_stop.second.AsInt());
			}
		}
	}

	// buses insert
	for (Node base_data : base.AsMap().at("base_requests").AsArray()) {
		if (base_data.AsMap().at("type").AsString() == "Bus") {
			Bus bus;
			bus.bus_name = base_data.AsMap().at("name").AsString();
			bus.is_ring = base_data.AsMap().at("is_roundtrip").AsBool();
			for (Node stop : base_data.AsMap().at("stops").AsArray()) {
				bus.stops_unique.insert(catalogue.FindStop(stop.AsString()));
				bus.stops_vector.push_back(catalogue.FindStop(stop.AsString()));
			}
			catalogue.AddBus(std::move(bus));
		}
	}

	// stat result
	Array result;
	for (Node stat_data : base.AsMap().at("stat_requests").AsArray()) {
		if (stat_data.AsMap().at("type").AsString() == "Stop") {
			//catalogue.AddStopInfo(stat_data.AsMap().at("name").AsString());
			result.push_back(MakeDictStop(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString(), catalogue));
		}
		else if (stat_data.AsMap().at("type").AsString() == "Bus") {
			//catalogue.AddBusInfo(stat_data.AsMap().at("name").AsString());
			result.push_back(MakeDictBus(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString(), catalogue));
		}
	}
	Document doc(std::move(result));
	Print(doc, output);
}

