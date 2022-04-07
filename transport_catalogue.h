#pragma once

#include "domain.h"
#include "geo.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <set>
#include <string>
#include <functional>
#include <string>

namespace transport_catalogue {
	using BusToFindType = std::unordered_map<std::string_view, Bus*, Hasher>;
	using StopToFindType = std::unordered_map <std::string_view, Stop*, Hasher>;
	using BusInfoType = std::unordered_map<std::string_view, BusInfo, Hasher>;
	using RouteLengthsType = std::unordered_map<std::pair<Stop*, Stop*>, uint64_t, Hasher>;
	//using StopInfoType = std::unordered_map<std::pair<std::string_view, Stop*>, std::set<Bus*>, Hasher>;
	//using StopInfoType = std::unordered_map<std::pair<std::string_view, Stop*>, std::set<std::string>, Hasher>;
	using StopInfoType = std::unordered_map<Stop*, std::unordered_set<Bus*>, Hasher>;

	class TransportCatalogue {
	public:
		void AddBus(Bus bus) {
			buses_.push_back(std::move(bus));
			buses_to_find_[buses_.back().bus_name] = &buses_.back();
			// add stop_info_
			for (Stop* stop : buses_.back().stops_vector) {
				stop_info_[stop].insert(FindBus(buses_.back().bus_name));
			}
			// add bus_info_
			AddBusInfo();
			/*bus_info_[buses_.back().bus_name].bus_name = buses_.back().bus_name;
			bus_info_[buses_.back().bus_name].unique_stops = buses_.back().stops_unique.size();

			uint64_t full_lng = 0;
			for (int i = 1; i < buses_.back().stops_vector.size(); ++i) {
			bus_info_[buses_.back().bus_name].route_length += geo::ComputeDistance(buses_.back().stops_vector[i-1]->coodinates , buses_.back().stops_vector[i]->coodinates);
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
			}*/
		}

		void AddStop(Stop stop) {
			stops_.push_back(std::move(stop));
			stops_to_find_[stops_.back().stop_name] = &stops_.back();
		}

		Bus* FindBus(std::string_view bus_number);

		Stop* FindStop(std::string_view stop_name);

	//	void AddStopInfo(std::string_view stop_name);

	//	void AddBusInfo(std::string_view bus_name);

		BusInfoType GetBusInfo() {
			return bus_info_;
		}

		StopInfoType GetStopInfo() {
			return stop_info_;
		}

		void SetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop, uint64_t length);

		uint64_t GetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop);

	private:

		void AddBusInfo();

		std::deque<Bus> buses_;
		std::deque<Stop> stops_;

		//std::unordered_map<Stop*, std::unordered_set<Bus*>> bus_by_stop_;
		BusToFindType buses_to_find_;
		StopToFindType stops_to_find_;
		BusInfoType bus_info_;
		RouteLengthsType route_lengths_;
		//StopInfoType stop_info_;
		StopInfoType stop_info_;
	};
}