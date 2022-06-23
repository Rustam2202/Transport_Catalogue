#include "request_handler.h"

#include <algorithm>
#include <execution>

Node RequestHandler::MakeDictStop(int request_id, std::string_view stop_name) {
	const StopInfoType& stop_info = catalogue_.GetStopInfo();
	Stop* stop_finded = catalogue_.FindStop(stop_name);

	if (stop_finded == nullptr) {
		return Builder{}.StartDict()
			.Key("request_id"s).Value(request_id)
			.Key("error_message"s).Value("not found"s)
			.EndDict().Build();
	}
	else {
		Array buses_arr;
		if (stop_info.count(stop_finded)) {
			std::unordered_set<Bus*> buses = stop_info.at(stop_finded);
			for (Bus* bus : buses) {
				buses_arr.push_back(bus->bus_name);
			}
			std::sort(std::execution::par, buses_arr.begin(), buses_arr.end(),
				[](const Node& lhs, const Node& rhs) {return lhs.AsString() < rhs.AsString(); });
		}
		return Builder{}.StartDict()
			.Key("buses"s).Value(buses_arr)
			.Key("request_id"s).Value(request_id)
			.EndDict().Build();
	}
}

Node RequestHandler::MakeDictBus(int request_id, std::string_view bus_name) {
	const BusInfoType& bus_info = catalogue_.GetBusInfo();
	Bus* bus_finded = catalogue_.FindBus(bus_name);

	if (bus_finded == nullptr) {
		return Builder{}.StartDict()
			.Key("request_id"s).Value(request_id)
			.Key("error_message").Value("not found"s)
			.EndDict().Build();
	}
	else {
		const BusInfo& bus_info_finded = bus_info.at(bus_name);
		return Builder{}.StartDict()
			.Key("curvature"s).Value(bus_info_finded.curvature)
			.Key("request_id"s).Value(request_id)
			.Key("route_length"s).Value((double)bus_info_finded.route_length_on_road)
			.Key("stop_count"s).Value((int)bus_info_finded.stops_on_route)
			.Key("unique_stop_count"s).Value((int)bus_info_finded.unique_stops)
			.EndDict().Build();
	}
}

Node RequestHandler::MakeDictMap(int request_id) {
	std::stringstream s;
	renderer_.Rendering(s);
	return Builder{}.StartDict()
		.Key("map"s).Value(s.str())
		.Key("request_id"s).Value(request_id)
		.EndDict().Build();
}

Node RequestHandler::MakeDictRoute(int request_id, std::string_view from, std::string_view to) {
	auto route = router_.BuildRoute(from, to);
	double total_time = 0.0;
	return Builder{}.StartDict()
		.Key("items")
		.StartArray()

		.EndArray()
		.EndDict().Build();
}

void RequestHandler::SetZoom() {
	std::vector<geo::Coordinates> result;
	for (const auto& stop : catalogue_.GetStopInfo()) {
		if (!stop.second.empty()) {
			result.push_back(stop.first->coodinates);
		}
	}
	renderer_.MakeSphereProjector(result);
}

void RequestHandler::AddBusesData() {
	for (const Bus& bus : catalogue_.GetBuses()) {
		if (bus.stops_vector.empty()) {
			continue;
		}
		for (const Stop* stop : bus.stops_vector) {
			renderer_.AddBusWithStops(bus.bus_name, bus.is_ring, stop->stop_name, stop->coodinates);
		}
	}
}

void RequestHandler::DrawMap() {
	renderer_.RenderBusesLines();
	renderer_.Sorting();
	renderer_.RenderBusesNames();
	renderer_.RenderCircle();
	renderer_.RenderStopsNames();
}
