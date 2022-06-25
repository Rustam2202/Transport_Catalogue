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

struct WaitAndMovement {
	double wait = 0;
	double movement = 0;

	bool operator<(const WaitAndMovement& rhs) const {
		return movement + wait < rhs.movement + rhs.wait;
	}
	bool operator>(const WaitAndMovement& rhs) const {
		return movement + wait > rhs.movement + rhs.wait;
	}
};

inline WaitAndMovement operator+(const WaitAndMovement& lhs, const WaitAndMovement& rhs) {
	return { lhs.wait + rhs.wait,lhs.movement + rhs.movement };
}

struct RouteInfo {
	std::optional<std::string_view> stop_name;
	std::optional<std::string_view> bus_name;
	std::optional<int> span_count;
	/*std::string_view stop_name;
	std::string_view bus_name;
	size_t span_count;*/
	double time;
};

class TransportGraph {
	using StopIdInfo = std::unordered_map<std::string_view, int, Hasher>;
	using GraphType = graph::DirectedWeightedGraph<WaitAndMovement>;
	using EdgeIdToBusName = std::vector<std::string_view>; // index = edgeId, value = bus_name

public:
	TransportGraph(TransportCatalogue& catalogue, int wait_time, int velocity);

	const GraphType& GetGraph() { return graph_; }
	size_t GetIdOfStopName(std::string_view name) { return stops_name_to_id_.at(name); }
	std::string_view GetBusNameById(size_t id) { return id_to_bus_name_.at(id); }

private:
	double CalculateMoveWeight(uint64_t dist) {
		return (static_cast<double>(60) * dist) / (1000 * bus_velocity_);
	}

	int bus_wait_time_ = 0;
	int bus_velocity_ = 0;
	StopIdInfo stops_name_to_id_;
	EdgeIdToBusName id_to_bus_name_;
	GraphType graph_;
};

class TransportRoter :public TransportGraph {
public:
	TransportRoter(TransportCatalogue& catalogue, int wait_time, int velocity) :
		TransportGraph(catalogue, wait_time, velocity),
		stops_(catalogue.GetStops()),
		router_(GetGraph())
	{}

	std::vector<RouteInfo> BuildRoute(std::string_view from, std::string_view to) {
		std::vector<RouteInfo> result;
		auto route = router_.BuildRoute(GetIdOfStopName(from), GetIdOfStopName(to));
		if (route == std::nullopt) {
			return result;
		}
		for (size_t edge_id : route.value().edges) {
			//auto& temp = GetGraph().GetEdge(edge_id);
			RouteInfo wait_part;
			wait_part.stop_name = stops_.at(GetGraph().GetEdge(edge_id).from).stop_name;
			wait_part.time = route.value().weight.wait;
			result.push_back(wait_part);
			RouteInfo move_part;
			move_part.bus_name = GetBusNameById(edge_id);
			move_part.time = route.value().weight.movement;
			move_part.span_count = GetGraph().GetEdge(edge_id).span_count;
			result.push_back(move_part);

			//auto bus_name = GetBusNameById(edge_id);
			//auto& stop_name = stops_.at(GetGraph().GetEdge(edge_id).from);
		}
		return result;
	}

private:
	const std::deque<Stop>& stops_;
	graph::Router<WaitAndMovement> router_;
};
