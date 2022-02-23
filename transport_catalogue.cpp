
#include "transport_catalogue.h"

Bus* TransportCatalogue::FindBus(string bus_number) {
	auto it = find_if(buses_.begin(), buses_.end(),
		[bus_number](const Bus& bus) {
			return bus.bus == bus_number;
		}
	);
	if (it == buses_.end()) {
		return nullptr;
	}
	return &buses_[it - buses_.begin()];
}

Stop* TransportCatalogue::FindStop(std::string_view str) {
	auto it = std::find_if(stops_.begin(), stops_.end(),
		[str](const Stop& stop) {
			return stop.stop == str;
		}
	);

	if (it == stops_.end()) {
		return nullptr;
	}

	return &stops_[it - stops_.begin()];
}

BusInfo TransportCatalogue::GetBusInfo(string bus_number) {
	BusInfo result;
	Bus* bus_finded = FindBus(bus_number);

	result.bus = bus_number;

	if (bus_finded == nullptr) {
		return result;
	}

	result.unique_stops = bus_finded->stops_unique.size();

	for (int i = 1; i < bus_finded->stops_vector.size(); ++i) {
		result.route_length += ComputeDistance(bus_finded->stops_vector[i - 1]->coodinates, bus_finded->stops_vector[i]->coodinates);
	}

	if (bus_finded->IsRing == true) {
		result.stops_on_route = bus_finded->stops_vector.size();

	}
	else {
		result.stops_on_route = bus_finded->stops_vector.size() * 2 - 1;
		result.route_length *= 2;
	}

	return result;
}