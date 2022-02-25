#pragma once

#include "geo.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>

using namespace std;

struct Stop
{
	std::string stop;
	Coordinates coodinates;
};

struct Bus
{
	std::string bus;
	std::unordered_set<Stop*> stops_unique;
	//	std::unordered_map<Stop*, size_t> stops;

	std::vector<Stop*> stops_vector;
	bool IsRing = false;
};

struct BusInfo {
	string bus;
	size_t stops_on_route = 0;
	size_t unique_stops = 0;
	double route_length = 0.0;
};

struct StopInfo {
	string stop_name;
	set<string> stop_with_buses;
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
	Bus* FindBus(string bus_number);

	//	поиск остановки по имени
	Stop* FindStop(std::string_view str);

	//	получение информации о маршруте
	BusInfo GetBusInfo(string bus_number);

	// получение информации об остановке (пересекающие маршруты)
	StopInfo GetStopInfo(string stop_name);

	// задание дистанции между остановками
	void SetDistanceBetweenStops(Stop* this_stop, Stop* other_stop){

	}
	// Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino


	//получение дистанции между остановками
	void GetDistanceBetweenStops() {

	}

private:
	std::deque<Bus> buses_;
	std::deque<Stop> stops_;

	//unordered_map<Stop*, set<string>> stop_with_buses;
	std::unordered_map<std::pair<Stop*, Stop*>, uint64_t> route_lengths_;
};