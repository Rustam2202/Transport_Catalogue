#pragma once

#include "geo.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
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
	Bus* FindBus(string number) {
		auto it = find_if(buses_.begin(), buses_.end(),
			[number](const Bus& bus) {
				return bus.bus == number;
			}
		);
		if (it == buses_.end()) {
			return nullptr;
		}

		return &buses_[it - buses_.begin()];
	}

	//	поиск остановки по имени
	Stop* FindStop(std::string_view str) {
		auto it = std::find_if(stops_.begin(), stops_.end(),
			[str](const Stop& stop) {
				return stop.stop == str;
			}
		);

		return &stops_[it - stops_.begin()];
	}

	//	получение информации о маршруте
	BusInfo GetBusInfo(string number) {
		BusInfo result;
		Bus* bus_finded = FindBus(number);

		result.bus = number;

		if (bus_finded == nullptr) {
			return result;
		}

		result.unique_stops = bus_finded->stops_unique.size();

		for (int i = 1; i < bus_finded->stops_vector.size(); ++i) {
			result.route_length += ComputeDistance(bus_finded->stops_vector[i-1]->coodinates, bus_finded->stops_vector[i]->coodinates);
		}

		if (bus_finded->IsRing == true) {
			result.stops_on_route = bus_finded->stops_vector.size();

		}
		else {
			result.stops_on_route = bus_finded->stops_vector.size() * 2 - 1;
			result.route_length *= 2;
		}

		return result;
	}

private:
	std::deque<Bus> buses_;
	std::deque<Stop> stops_;
};

/*

6
Stop Stop1: 55.611087, 37.208290
Stop Stop2: 55.595884, 37.209755
Bus 4: Stop1 - Stop2
Bus 3 c: Stop1 > Stop2 > Stop2 > Stop2 > Stop1
Bus 2a: Stop1 > Stop2
Bus 1: Stop1 - Stop2 - Stop2 - Stop1

5
Bus 1
Bus 2a
Bus 3 c
Bus 4
Bus 5

10
Stop Tolstopaltsevo: 55.611087, 37.208290
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.651700
Stop Biryusinka: 55.581065, 37.648390
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751

*/