#include "transport_catalogue.h"

#include <algorithm>

namespace transport_catalogue {

	Bus* TransportCatalogue::FindBus(std::string bus_number) {
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

	void TransportCatalogue::FindBus2(std::string bus_number) {

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

	void TransportCatalogue::AddStopInfo(std::string stop_name) {

		Stop* stop_finded = FindStop(stop_name);

		if (stop_finded == nullptr) {
			stop_info_[{stop_name, nullptr}];
			return;
		}

		for (const Bus& bus : buses_) {
			if (bus.stops_unique.count(stop_finded) > 0) {
				stop_info_[{stop_name, stop_finded}].insert(bus.bus);
			}
			else {
				stop_info_[{stop_name, stop_finded}];
			}
		}
	}

	std::unordered_map<std::pair<std::string, Stop*>,
		std::set<std::string>,
		Hasher> TransportCatalogue::GetStopInfo() {
		return stop_info_;
	}

	BusInfo	TransportCatalogue::GetBusInfo(std::string bus_number) {
		BusInfo result;
		Bus* bus_finded = FindBus(bus_number);

		result.bus = bus_number;

		if (bus_finded == nullptr) {
			return result;
		}

		/*std::vector<Stop*> temp_unique;
		temp_unique.reserve(10);
		std::unique_copy(bus_finded->stops_vector.begin(), bus_finded->stops_vector.end(), temp_unique.begin());
		result.unique_stops = temp_unique.size();*/

		result.unique_stops = bus_finded->stops_unique.size();

		uint64_t full_lng = 0;
		for (int i = 1; i < bus_finded->stops_vector.size(); ++i) {
			result.route_length += geo::ComputeDistance(bus_finded->stops_vector[i - 1]->coodinates, bus_finded->stops_vector[i]->coodinates);
			full_lng += GetDistanceBetweenStops(bus_finded->stops_vector[i - 1]->stop, bus_finded->stops_vector[i]->stop);
		}
		if (bus_finded->is_ring == false) {
			for (int i = bus_finded->stops_vector.size() - 1; i > 0; --i) {
				result.route_length += geo::ComputeDistance(bus_finded->stops_vector[i]->coodinates, bus_finded->stops_vector[i - 1]->coodinates);
				full_lng += GetDistanceBetweenStops(bus_finded->stops_vector[i]->stop, bus_finded->stops_vector[i - 1]->stop);
			}
		}

		result.route_length_on_road = full_lng;
		result.curvature = full_lng / result.route_length;

		if (bus_finded->is_ring == true) {
			result.stops_on_route = bus_finded->stops_vector.size();

		}
		else {
			result.stops_on_route = bus_finded->stops_vector.size() * 2 - 1;
			result.route_length *= 2;
		}
		return result;
	}

	void TransportCatalogue::SetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop, uint64_t length) {
		auto finded_this = FindStop(this_stop);
		auto finded_other = FindStop(other_stop);
		if (finded_this != nullptr && finded_other != nullptr) {
			route_lengths_[{finded_this, finded_other}] = length;
		}
	}

	uint64_t TransportCatalogue::GetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop) {
		auto finded_this = FindStop(this_stop);
		auto finded_other = FindStop(other_stop);
		if (finded_this != nullptr && finded_other != nullptr) {
			auto finded_lng_it = route_lengths_.find({ finded_this, finded_other });
			if (finded_lng_it == route_lengths_.end()) {
				return route_lengths_.at({ finded_other,finded_this });
			}
			return route_lengths_.at({ finded_this, finded_other });
		}
		return 0;
	}
}