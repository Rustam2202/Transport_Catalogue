#pragma once

#include "geo.h"

#include <algorithm>
#include <deque>
#include <unordered_set>
#include <string>

struct Stop
{
	std::string stop;
	Coordinates coodinates;
};

struct Bus
{
	int bus;
	std::unordered_set<Stop*> stops;
	bool IsRing = false;
	//double distance = 0.0;
};

struct BusInfo {
	int bus;
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
	Bus* FindBus(int number) {
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
	BusInfo GetBusInfo(int number) {
		BusInfo result;
		Bus* bus_p = FindBus(number);
		if (bus_p == nullptr) {
			return result;
		}

		result.bus = number;
		auto it = bus_p->stops.begin();
		auto it_begin = bus_p->stops.begin();
		it++;
		while (it != bus_p->stops.end()) {
			result.route_length += ComputeDistance((*prev(it))->coodinates, (*it)->coodinates);
			it++;
		}

		if (bus_p->IsRing == true) {
			result.route_length += ComputeDistance((*prev(it))->coodinates, (*it_begin)->coodinates);
			result.stops_on_route = bus_p->stops.size() + 1;
		}
		else {
			result.route_length *= 2;
			result.stops_on_route = bus_p->stops.size() * 2 - 1;
		}
		result.unique_stops = bus_p->stops.size();

		return result;
	}

private:
	std::deque<Bus> buses_;
	std::deque<Stop> stops_;
};

/*

10
Stop Tolstopaltsevo : 55.611087, 37.208290
Stop Marushkino : 55.595884, 37.209755
Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka : 55.632761, 37.333324
Stop Biryulyovo Zapadnoye : 55.574371, 37.651700
Stop Biryusinka : 55.581065, 37.648390
Stop Universam : 55.587655, 37.645687
Stop Biryulyovo Tovarnaya : 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751

*/