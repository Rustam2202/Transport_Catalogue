#include "geo.h"

#include <deque>
#include <unordered_set>
#include <string>

class TransportCatalogue {
public:

	//	добавление маршрута в базу
	void AddBus();

	//	добавление остановки в базу
	void AddStop();

	//	поиск маршрута по имени
	void FindBus();

	//	поиск остановки по имени
	void FindStop();

	//	получение информации о маршруте
	void GetBusInfo();


private:
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
	
	std::deque<Bus> buses_;
	std::deque<Stop> stops_;
};