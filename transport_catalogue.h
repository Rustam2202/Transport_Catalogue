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
	using StopInfoType = std::unordered_map<Stop*, std::unordered_set<Bus*>, Hasher>;

	class TransportCatalogue {
		//friend class TransportRouter;

	public:
		void AddBus(Bus bus);

		void AddStop(Stop stop);

		Bus* FindBus(std::string_view bus_number);

		Stop* FindStop(std::string_view stop_name);

		const std::deque<Bus>& GetBuses() { return buses_; }

		const std::deque<Stop>& GetStops() { return stops_; }

		BusInfoType& GetBusInfo() { return bus_info_; }

		StopInfoType& GetStopInfo() { return stop_info_; }

		void SetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop, uint64_t length);

		uint64_t GetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop);

	private:
		void AddBusInfo();

		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		BusToFindType buses_to_find_;
		StopToFindType stops_to_find_;
		BusInfoType bus_info_;
		StopInfoType stop_info_;
		RouteLengthsType route_lengths_;
	};
}