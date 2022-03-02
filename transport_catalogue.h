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
#include <functional>
#include <string>

namespace transport_catalogue {

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
		bool is_ring = false;
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
		bool is_in_stop = true;
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
		Stop* FindStop(std::string str);

		//	получение информации о маршруте
		BusInfo GetBusInfo(std::string bus_number);

		BusInfo	GetBusInfoWithLengths(std::string bus_number);

		// получение информации об остановке (пересекающие маршруты)
		StopInfo GetStopInfo(std::string stop_name);
		void AddStopInfo(std::string stop_name);
		std::set<std::string> GetStopInfo2(const std::string& stop_name) const;

		// задание дистанции между остановками
		void SetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop, uint64_t length);

		//получение дистанции между остановками
		uint64_t GetDistanceBetweenStops(std::string_view this_stop, std::string_view other_stop);

	private:

		class Hasher {
		public:
			size_t operator()(std::pair<Stop*, Stop*> stops) const {
				return string_hasher_(stops.first->stop + stops.second->stop);
			}
			size_t operator()(std::pair<std::string, Stop*> stops) const {
				if (stops.second == nullptr) {
					return string_hasher_(stops.first);
				}
				return string_hasher_(stops.first + stops.second->stop);
			}
		private:
			std::hash<std::string> string_hasher_;
		};

		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		//std::map<std::pair<Stop*, Stop*>, uint64_t> route_lengths_;

		std::unordered_map<std::pair<Stop*, Stop*>, uint64_t, Hasher> route_lengths_;
		std::unordered_map<std::pair<std::string, Stop*>, std::set<std::string>, Hasher> stop_info_;
	};
}