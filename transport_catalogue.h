#pragma once

#include "domain.h"
#include "geo.h"

#include <deque>
#include <iostream>
#include <unordered_map>
#include <set>
#include <string>
#include <functional>
#include <string>

namespace transport_catalogue {
	using BusToFindType = std::unordered_map<std::string_view, Bus*, Hasher>;
	using StopToFindType = std::unordered_map<std::string_view, Stop*, Hasher>;
	using BusInfoType = std::unordered_map<std::string_view, BusInfo, Hasher>;
	using RouteLengthsType = std::unordered_map<std::pair<Stop*, Stop*>, uint64_t, Hasher>;
	//using StopInfoType = std::unordered_map<std::pair<std::string_view, Stop*>, std::set<Bus*>, Hasher>;
	using StopInfoType = std::unordered_map<std::pair<std::string_view, Stop*>, std::set<std::string>, Hasher>;

	class TransportCatalogue {
	public:
		void AddBus(Bus bus) {
			buses_.push_back(bus);
		}

		void AddStop(Stop stop) {
			stops_.push_back(stop);
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

		void SetDistanceBetweenStops(const std::string& this_stop, const std::string& other_stop, uint64_t length);

		uint64_t GetDistanceBetweenStops(const std::string& this_stop, const std::string& other_stop);

	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;

		BusToFindType buses_to_find_;
		StopToFindType stops_to_find_;
		BusInfoType bus_info_;
		RouteLengthsType route_lengths_;
		StopInfoType stop_info_;
	};
}