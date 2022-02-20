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
	// int bus;
	size_t stops_on_route;
	size_t unique_stops;
	double route_length=0.0;
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

		return &buses_[it - buses_.end()];
	}

	//	поиск остановки по имени
	Stop* FindStop(std::string_view str) {
		auto it = std::find_if(stops_.begin(), stops_.end(),
			[str](const Stop& stop) {
				return stop.stop == str;
			}
		);

		return &stops_[it - stops_.end()];
	}

	//	получение информации о маршруте
	BusInfo GetBusInfo(int number) {
		BusInfo result;
		Bus* bus_p = FindBus(number);

		auto it = bus_p->stops.begin();
		while (it != bus_p->stops.end()--) {
			result.route_length += ComputeDistance((*it)->coodinates, (*it + 1)->coodinates);
			it++;
		}

		if (bus_p->IsRing == true) {
			result.stops_on_route = bus_p->stops.size() + 1;
		}
		else {
			result.stops_on_route = bus_p->stops.size() * 2 - 1;
			result.route_length *= 2;

		}
		result.unique_stops = bus_p->stops.size();

		return result;
	}

private:
	std::deque<Bus> buses_;
	std::deque<Stop> stops_;
};