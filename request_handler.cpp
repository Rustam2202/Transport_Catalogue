#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

void RequestHandler::InsertStops(Array base) {
	for (Node base_data : base) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Stop stop;
			stop.stop_name = base_data.AsMap().at("name").AsString();
			stop.coodinates.lat = base_data.AsMap().at("latitude").AsDouble();
			stop.coodinates.lng = base_data.AsMap().at("longitude").AsDouble();
			catalogue_.AddStop(std::move(stop));
		}
	}
}

void RequestHandler::InsertStopsDistances(json::Array base) {
	for (Node base_data : base) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Dict stops = base_data.AsMap().at("road_distances").AsMap();
			for (auto other_stop : stops) {
				catalogue_.SetDistanceBetweenStops(base_data.AsMap().at("name").AsString(), other_stop.first, other_stop.second.AsInt());
			}
		}
	}
}

void RequestHandler::InsertBuses(json::Array base) {
	for (Node base_data : base) {
		if (base_data.AsMap().at("type").AsString() == "Bus") {
			Bus bus;
			bus.bus_name = base_data.AsMap().at("name").AsString();
			bus.is_ring = base_data.AsMap().at("is_roundtrip").AsBool();
			for (Node stop : base_data.AsMap().at("stops").AsArray()) {
				bus.stops_unique.insert(catalogue_.FindStop(stop.AsString()));
				bus.stops_vector.push_back(catalogue_.FindStop(stop.AsString()));
			}
			catalogue_.AddBus(std::move(bus));
		}
	}
}

void RequestHandler::CompileStats(json::Array base, json::Array& stats) {
	for (Node stat_data : base) {
		if (stat_data.AsMap().at("type").AsString() == "Stop") {
			stats.push_back(MakeDictStop(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString()));
		}
		else if (stat_data.AsMap().at("type").AsString() == "Bus") {
			stats.push_back(MakeDictBus(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString()));
		}
	}
}

Dict RequestHandler::MakeDictStop(int request_id, std::string_view stop_name) {
	const StopInfoType& stop_info = catalogue_.GetStopInfo();
	Stop* stop_finded = catalogue_.FindStop(stop_name);

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

Dict RequestHandler::MakeDictBus(int request_id, std::string_view bus_name) {
	const BusInfoType& bus_info = catalogue_.GetBusInfo();
	Bus* bus_finded = catalogue_.FindBus(bus_name);

	if (bus_finded == nullptr) {
		return { {"request_id"s, request_id}, {"error_message"s, "not found"s} };
	}
	else {
		const BusInfo& bus_info_finded = bus_info.at(bus_name);
		return{
			{"curvature"s, bus_info_finded.curvature},
			{"request_id"s, request_id},
			{ "route_length"s, (double)bus_info_finded.route_length_on_road},
			{"stop_count"s, (int)bus_info_finded.stops_on_route},
			{"unique_stop_count"s, (int)bus_info_finded.unique_stops}
		};
	}
}