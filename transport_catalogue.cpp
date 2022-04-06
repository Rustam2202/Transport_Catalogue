#include "transport_catalogue.h"

#include <algorithm>

namespace transport_catalogue {

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

	void TransportCatalogue::AddBusInfo(std::string_view bus_name) {
		Bus* bus_finded = FindBus(bus_name);

		if (bus_finded == nullptr) {
			return;
		}
		if (bus_info_.count(bus_name) > 0) {
			return;
		}

		bus_info_[bus_name].bus_name = bus_name;
		bus_info_[bus_name].unique_stops = bus_finded->stops_unique.size();

		uint64_t full_lng = 0;
		for (int i = 1; i < bus_finded->stops_vector.size(); ++i) {
			bus_info_[bus_name].route_length += geo::ComputeDistance(bus_finded->stops_vector[i - 1]->coodinates, bus_finded->stops_vector[i]->coodinates);
			full_lng += GetDistanceBetweenStops(bus_finded->stops_vector[i - 1]->stop_name, bus_finded->stops_vector[i]->stop_name);
		}
		if (bus_finded->is_ring == false) {
			for (int i = bus_finded->stops_vector.size() - 1; i > 0; --i) {
				bus_info_[bus_name].route_length += geo::ComputeDistance(bus_finded->stops_vector[i]->coodinates, bus_finded->stops_vector[i - 1]->coodinates);
				full_lng += GetDistanceBetweenStops(bus_finded->stops_vector[i]->stop_name, bus_finded->stops_vector[i - 1]->stop_name);
			}
		}

		bus_info_[bus_name].route_length_on_road = full_lng;
		bus_info_[bus_name].curvature = full_lng / bus_info_[bus_name].route_length;

		if (bus_finded->is_ring == true) {
			bus_info_[bus_name].stops_on_route = bus_finded->stops_vector.size();

		}
		else {
			bus_info_[bus_name].stops_on_route = bus_finded->stops_vector.size() * 2 - 1;
			bus_info_[bus_name].route_length *= 2;
		}
	}

	//void TransportCatalogue::AddStopInfo(std::string_view stop_name) {
	//	Stop* stop_finded = FindStop(stop_name);
	//	if (stop_finded == nullptr) {
	//		return;
	//	}
	//	for (Bus bus : buses_) {
	//		if (bus.stops_unique.count(stop_finded) > 0) {
	//			//stop_info_[{stop_name, stop_finded}].insert(bus.bus);
	//			// 
	//			//stop_info_[{stop_name, stop_finded}].insert(&bus);
	//		}
	//		else {
	//			//stop_info_[{stop_name, stop_finded}];
	//		}
	//	}
	//	/*for (auto it = buses_.begin(); it != buses_.end(); it++) {
	//		if ((*it).stops_unique.count(stop_finded) > 0) {
	//			stop_info_[{stop_name, stop_finded}].insert(&(*it));
	//		}
	//		else {
	//			stop_info_[{stop_name, stop_finded}].insert(nullptr);
	//		}
	//	}*/
	//}

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