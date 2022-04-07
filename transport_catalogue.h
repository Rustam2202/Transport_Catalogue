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
			for (Stop* stop : buses_.back().stops_vector) {
				stop_info_[stop].insert(FindBus(buses_.back().bus_name));
			}
		}

		void AddStop(Stop stop) {
			stops_.push_back(stop);
			stops_to_find_[stops_.back().stop_name] = &stops_.back();
		}

		Bus* FindBus(std::string_view bus_number);

		Stop* FindStop(std::string_view stop_name);

		void AddStopInfo(std::string_view stop_name);

		void AddBusInfo(std::string_view bus_name);

		BusInfoType GetBusInfo() {
			return bus_info_;
		}

		StopInfoType GetStopInfo() {
			return stop_info_;
		}

		void SetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop, uint64_t length);

		uint64_t GetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop);

	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;

		std::unordered_map<Stop*, std::unordered_set<Bus*>> bus_by_stop_;
		BusToFindType buses_to_find_;
		StopToFindType stops_to_find_;
		BusInfoType bus_info_;
		RouteLengthsType route_lengths_;
		//StopInfoType stop_info_;
		StopInfoType stop_info_;
	};
}