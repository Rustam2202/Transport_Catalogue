#include "transport_catalogue.h"

#include <algorithm>

namespace transport_catalogue {

	void TransportCatalogue::AddBus(Bus bus) {
		buses_.push_back(std::move(bus));
		buses_to_find_[buses_.back().bus_name] = &buses_.back();
		for (Stop* stop : buses_.back().stops_vector) {
			stop_info_[stop].insert(FindBus(buses_.back().bus_name));
		}
		AddBusInfo();
	}

	void TransportCatalogue::AddStop(Stop stop) {
		stops_.push_back(std::move(stop));
		stops_to_find_[stops_.back().stop_name] = &stops_.back();
	}

	Bus* TransportCatalogue::FindBus(std::string_view bus_number) {
		auto it = buses_to_find_.find(bus_number);
		if (it != buses_to_find_.end()) {
			return (*it).second;
		}
		else {
			return nullptr;
		}
	}

	Stop* TransportCatalogue::FindStop(std::string_view stop_name) {
		auto it = stops_to_find_.find(stop_name);
		if (it != stops_to_find_.end()) {
			return (*it).second;
		}
		else {
			return nullptr;
		}
	}

	void TransportCatalogue::AddBusInfo() {
		bus_info_[buses_.back().bus_name].bus_name = buses_.back().bus_name;
		bus_info_[buses_.back().bus_name].unique_stops = buses_.back().stops_unique.size();

		uint64_t full_lng = 0;
		for (size_t i = 1; i < buses_.back().stops_vector.size(); ++i) {
			bus_info_[buses_.back().bus_name].route_length += geo::ComputeDistance(buses_.back().stops_vector[i - 1]->coodinates, buses_.back().stops_vector[i]->coodinates);
			full_lng += GetDistanceBetweenStops(buses_.back().stops_vector[i - 1]->stop_name, buses_.back().stops_vector[i]->stop_name);
		}
		if (buses_.back().is_ring == false) {
			for (int i = buses_.back().stops_vector.size() - 1; i > 0; --i) {
				bus_info_[buses_.back().bus_name].route_length += geo::ComputeDistance(buses_.back().stops_vector[i]->coodinates, buses_.back().stops_vector[i - 1]->coodinates);
				full_lng += GetDistanceBetweenStops(buses_.back().stops_vector[i]->stop_name, buses_.back().stops_vector[i - 1]->stop_name);
			}
		}

		bus_info_[buses_.back().bus_name].route_length_on_road = full_lng;
		bus_info_[buses_.back().bus_name].curvature = full_lng / bus_info_[buses_.back().bus_name].route_length;

		if (buses_.back().is_ring == true) {
			bus_info_[buses_.back().bus_name].stops_on_route = buses_.back().stops_vector.size();

		}
		else {
			bus_info_[buses_.back().bus_name].stops_on_route = buses_.back().stops_vector.size() * 2 - 1;
			bus_info_[buses_.back().bus_name].route_length *= 2;
		}
	}

	void TransportCatalogue::SetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop, uint64_t length) {
		Stop* finded_this = FindStop(this_stop);
		Stop* finded_other = FindStop(other_stop);
		if (finded_this != nullptr && finded_other != nullptr) {
			route_lengths_[{finded_this, finded_other}] = length;
		}
	}

	uint64_t TransportCatalogue::GetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop) {
		Stop* finded_this = FindStop(this_stop);
		Stop* finded_other = FindStop(other_stop);
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