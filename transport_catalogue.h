#pragma once

#include "geo.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>

namespace TransportCatalogueNamespace {
	struct Stop
	{
		std::string stop;
		Coordinates coodinates;
	};

	struct Bus
	{
		std::string bus;
		std::unordered_set<Stop*> stops_unique;
		std::vector<Stop*> stops_vector;
		bool IsRing = false;
	};

	struct BusInfo {
		std::string bus;
		size_t stops_on_route = 0;
		size_t unique_stops = 0;
		double route_length = 0.0;
		uint64_t route_length_on_road = 0;
		double curvature = 1.0;
	};

	struct StopInfo {
		std::string stop_name;
		std::set<std::string> stop_with_buses;
		bool IsInStops = true;
	};

	class TransportCatalogue {

	public:
		//	добавление маршрута в базу
		void AddBus(Bus bus) {
			buses_.push_back(bus);
		}

		//	добавление остановки в базу
		void AddStop(Stop stop) {
			stops_.push_back(stop);
		}

		//	поиск маршрута по имени
		Bus* FindBus(std::string bus_number);

		//	поиск остановки по имени
		Stop* FindStop(std::string_view str);

		//	получение информации о маршруте
		BusInfo GetBusInfo(std::string bus_number);

		BusInfo	GetBusInfoWithLengths(std::string bus_number);

		// получение информации об остановке (пересекающие маршруты)
		StopInfo GetStopInfo(std::string stop_name);

		// задание дистанции между остановками
		void SetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop, uint64_t length);

		//получение дистанции между остановками
		uint64_t GetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop);

	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::map<std::pair<Stop*, Stop*>, uint64_t> route_lengths_;
	};
}