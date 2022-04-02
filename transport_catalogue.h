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
	//	Bus* FindBus(std::string bus_number);
		Bus* FindBus(const std::string& bus_number);

		//	поиск остановки по имени
		//Stop* FindStop(std::string_view str);
		Stop* FindStop(const std::string& stop_name);

		//	получение информации о маршруте
		std::unordered_map<std::string, BusInfo, Hasher> GetBusInfo2();
		BusInfo	GetBusInfo(std::string bus_number);

		void AddStopInfo(std::string stop_name);

		void AddBusInfo(std::string bus_name);

		// получение информации об остановке (пересекающие маршруты)
		std::unordered_map<std::pair<std::string, Stop*>, std::set<std::string>, Hasher> GetStopInfo();

		// задание дистанции между остановками
		void SetDistanceBetweenStops(const std::string& this_stop, const std::string& other_stop, uint64_t length);

		//получение дистанции между остановками
		uint64_t GetDistanceBetweenStops(const std::string& this_stop, const std::string& other_stop);

	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;

		std::unordered_map<std::string, Bus*, Hasher> buses_to_find_;
		std::unordered_map<std::string, Stop*, Hasher> stops_to_find_;
		std::unordered_map<std::string, BusInfo, Hasher> bus_info_;
		//std::unordered_map<std::string, std::set<Stop*>, Hasher> stop_info_;
		//std::unordered_map<std::string, Coordinates, Hasher> stops_to_find_;
	//	std::unordered_map<std::string, std::pair<std::vector<Stop*>, bool>, Hasher> buses_to_find_;
		std::unordered_map<std::pair<Stop*, Stop*>, uint64_t, Hasher> route_lengths_;
		std::unordered_map<std::pair<std::string, Stop*>, std::set<std::string>, Hasher> stop_info_;
	};
}