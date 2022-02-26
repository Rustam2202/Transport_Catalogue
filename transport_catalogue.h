#pragma once

#include "geo.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <map> //
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
	std::vector<Stop*> stops_vector;
	bool IsRing = false;
};

struct BusInfo {
	string bus;
	size_t stops_on_route = 0;
	size_t unique_stops = 0;
	double route_length = 0.0;
	uint64_t route_length_on_road = 0;
	double curvature = 1.0;
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

	BusInfo	GetBusInfoWithLengths(string bus_number) {
		BusInfo result;
		Bus* bus_finded = FindBus(bus_number);

		result.bus = bus_number;

		if (bus_finded == nullptr) {
			return result;
		}

		result.unique_stops = bus_finded->stops_unique.size();

		uint64_t full_lng = 0;
		for (int i = 1; i < bus_finded->stops_vector.size(); ++i) {
			result.route_length += ComputeDistance(bus_finded->stops_vector[i - 1]->coodinates, bus_finded->stops_vector[i]->coodinates);
			full_lng += GetDistanceBetweenStops(bus_finded->stops_vector[i - 1]->stop, bus_finded->stops_vector[i]->stop);
		}
		if (bus_finded->IsRing == false) {
			for (int i = bus_finded->stops_vector.size() - 1; i > 0; --i) {
				result.route_length += ComputeDistance(bus_finded->stops_vector[i]->coodinates, bus_finded->stops_vector[i - 1]->coodinates);
				full_lng += GetDistanceBetweenStops(bus_finded->stops_vector[i]->stop, bus_finded->stops_vector[i - 1]->stop);
			}
		}

		result.route_length_on_road = full_lng;
		result.curvature = full_lng / result.route_length;

		if (bus_finded->IsRing == true) {
			result.stops_on_route = bus_finded->stops_vector.size();

		}
		else {
			result.stops_on_route = bus_finded->stops_vector.size() * 2 - 1;
			result.route_length *= 2;
		}

		return result;
	}

	// получение информации об остановке (пересекающие маршруты)
	StopInfo GetStopInfo(string stop_name);

	// задание дистанции между остановками
	void SetDistanceBetweenStops(string_view this_stop, string_view other_stop, uint64_t length) {
		//auto p = make_pair(this_stop, other_stop);
		auto finded_this = FindStop(this_stop);
		auto finded_other = FindStop(other_stop);
		if (finded_this != nullptr && finded_other != nullptr) {
			route_lengths_[{finded_this, finded_other}] = length;
		}
	}
	// Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino


	//получение дистанции между остановками
	uint64_t GetDistanceBetweenStops(string_view this_stop, string_view other_stop) {
		auto finded_this = FindStop(this_stop);
		auto finded_other = FindStop(other_stop);
		if (finded_this != nullptr && finded_other != nullptr) {
			auto finded_lng_it = route_lengths_.find({ finded_this, finded_other });
			if (finded_lng_it == route_lengths_.end()) {
				return route_lengths_.at({ finded_other,finded_this });
			}

			return route_lengths_.at({ finded_this, finded_other });
		}
		return 0;
	}

private:
	std::deque<Bus> buses_;
	std::deque<Stop> stops_;

	//unordered_map<Stop*, set<string>> stop_with_buses;
	std::map<std::pair<Stop*, Stop*>, uint64_t> route_lengths_; // unord_map!!
};