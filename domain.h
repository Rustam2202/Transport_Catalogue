#pragma once

#include "geo.h"

#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

namespace transport_catalogue {

	struct Stop
	{
		std::string stop_name;
		geo::Coordinates coodinates;
	};

	struct Bus
	{
		std::string bus_name;
		std::unordered_set<Stop*> stops_unique;
		std::vector<Stop*> stops_vector;
		bool is_ring = false;
	};

	struct BusInfo {
		std::string_view bus_name;
		size_t stops_on_route = 0;
		size_t unique_stops = 0;
		double route_length = 0.0;
		uint64_t route_length_on_road = 0;
		double curvature = 1.0;
	};

	/*struct StopInfo {
		std::string stop_name;
		std::set<std::string> stop_with_buses;
		bool is_in_stop = true;
	};*/

	class Hasher {
	public:

		size_t operator()(std::string_view name)const {
			return string_hasher_(name);
		}
		size_t operator()(Stop* stop) const {
			return string_hasher_(stop->stop_name);
		}

		size_t operator()(std::pair<Stop*, Stop*> stops) const {
			return string_hasher_(stops.first->stop_name + stops.second->stop_name);
		}

		size_t operator()(std::pair<std::string_view, Stop*> stops) const {
			if (stops.second == nullptr) {
				return string_hasher_(stops.first);
			}
			return string_hasher_(stops.first) + string_hasher_(stops.second->stop_name);
		}
		size_t operator()(std::pair<const std::string&, const std::string&> a)const {
			return string_hasher_(a.first + a.second);
		}

	private:
		std::hash<std::string_view> string_hasher_;
	};
}