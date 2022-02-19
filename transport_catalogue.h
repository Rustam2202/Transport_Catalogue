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
};

class TransportCatalogue {

public:

	//	добавление маршрута в базу
	void AddBus(Bus bus) {

	}

	//	добавление остановки в базу
	void AddStop(Stop stop) {
		stops_.push_back(stop);
	}

	//	поиск маршрута по имени
	void FindBus() {

	}

	//	поиск остановки по имени
	Stop* FindStop(std::string_view str) {
	/*_Deque_iterator*/ auto it = std::find_if(stops_.begin(), stops_.end(),
			[str](const Stop stop) {
				return stop.stop == str;
			}
		);
	auto a = *it;
		return nullptr;
	}

	//	получение информации о маршруте
	void GetBusInfo() {

	}



private:


	std::deque<Bus> buses_;
	std::deque<Stop> stops_;
};