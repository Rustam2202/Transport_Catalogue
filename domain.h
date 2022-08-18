#pragma once

#include "geo.h"

#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace transport_catalogue {

	struct Stop
	{
		uint32_t index = 0;
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
		uint32_t index = 0;
		std::string_view bus_name;
		size_t stops_on_route = 0;
		size_t unique_stops = 0;
		double route_length = 0.0;
		uint64_t route_length_on_road = 0;
		double curvature = 1.0;
	};

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
		size_t operator()(std::string_view name, int id)const {
			return string_hasher_(name) + id;
		}

	private:
		std::hash<std::string_view> string_hasher_;
	};
}