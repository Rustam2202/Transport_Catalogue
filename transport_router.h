#pragma once

#include "domain.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

#include <optional>
#include <vector>
#include <unordered_map>
#include <string_view>

using namespace transport_catalogue;

const int MIN_IN_HOUR = 60;
const int METERS_IN_KILOMETERS = 1000;

struct WeightInfo {
	double wait = 0;
	double movement = 0;

	bool operator<(const WeightInfo& rhs) const {
		return movement + wait < rhs.movement + rhs.wait;
	}
	bool operator>(const WeightInfo& rhs) const {
		return movement + wait > rhs.movement + rhs.wait;
	}
	bool operator==(const WeightInfo& rhs) const {
		return movement + wait == rhs.movement + rhs.wait;
	}
	bool operator!=(const WeightInfo& rhs) const {
		return !(*this == rhs);
	}
};

inline WeightInfo operator+(const WeightInfo& lhs, const WeightInfo& rhs) {
	return { lhs.wait + rhs.wait,lhs.movement + rhs.movement };
}

struct RouteInfo {
	std::optional<std::string_view> stop_name;
	std::optional<std::string_view> bus_name;
	std::optional<int> span_count;
	double time;
};

struct Routes {
	std::optional<std::vector<RouteInfo>> route;
	double weight;
};


class TransportGraph {
	using StopIdInfo = std::unordered_map<std::string_view, int, Hasher>;
	using GraphType = graph::DirectedWeightedGraph<WeightInfo>;
	using EdgeIdToBusName = std::vector<std::string_view>; // index = edgeId, value = bus_name

public:
	TransportGraph(TransportCatalogue& catalogue, int wait_time, int velocity);

	const GraphType& GetGraph() { return graph_; }
	size_t GetIdOfStopName(std::string_view name) { return stops_name_to_id_.at(name); }
	std::string_view GetBusNameById(size_t id) { return id_to_bus_name_.at(id); }

private:
	double CalculateMoveWeight(uint64_t dist) {
		return (static_cast<double>(MIN_IN_HOUR) * dist) / (METERS_IN_KILOMETERS * bus_velocity_);
	}
	void CalculateAndAddEdge(const Bus& bus, TransportCatalogue& catalogue,
		double& route_length, int k, int j, bool is_ring);

	int bus_wait_time_ = 0;
	int bus_velocity_ = 0;
	StopIdInfo stops_name_to_id_;
	EdgeIdToBusName id_to_bus_name_;
	GraphType graph_{};
};

class TransportRoter :public TransportGraph {
public:
	TransportRoter(TransportCatalogue& catalogue, int wait_time, int velocity) :
		TransportGraph(catalogue, wait_time, velocity),
		stops_(catalogue.GetStops()),
		router_(GetGraph())
	{}

	Routes BuildRoute(std::string_view from, std::string_view to);

private:
	const std::deque<Stop>& stops_;
	graph::Router<WeightInfo> router_;
};
