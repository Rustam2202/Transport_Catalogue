#pragma once

#include "domain.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

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

class TransportGraph {
	using StopIdInfo = std::unordered_map<std::string_view, int, Hasher>;
	using GraphType = graph::DirectedWeightedGraph<WaitAndMovement>;

public:
	TransportGraph(TransportCatalogue& catalogue, int wait_time, int velocity);

	const GraphType& GetGraph() { return graph_; }
	size_t GetIdOfStopName(std::string_view name) { return stops_name_and_id_.at(name); }

private:
	double CalculateMoveWeight(uint64_t dist) {
		return (static_cast<double>(60) * dist) / (1000 * bus_velocity_);
	}

	int bus_wait_time_ = 0;
	int bus_velocity_ = 0;
	StopIdInfo stops_name_and_id_;
	WaitAndMovement weight_;
	GraphType graph_;
};

class TransportRoter :public TransportGraph {
public:
	TransportRoter(TransportCatalogue& catalogue, int wait_time, int velocity) :
		TransportGraph(catalogue, wait_time, velocity),
		router_(GetGraph()) {}

	auto BuildRoute(std::string_view from, std::string_view to) {
		return router_.BuildRoute(GetIdOfStopName(from), GetIdOfStopName(to));
	}

private:
	graph::Router<WaitAndMovement> router_;
};
