#pragma once

#include "domain.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

#include <vector>
#include <unordered_map>
#include <string_view>

using namespace transport_catalogue;

struct WaitAndBus {
	double wait = 0;
	double movement = 0;

	bool operator<(const WaitAndBus& rhs) const {
		return movement + wait < rhs.movement + rhs.wait;
	}
	bool operator>(const WaitAndBus& rhs) const {
		return movement + wait > rhs.movement + rhs.wait;
	}
};

inline WaitAndBus operator+(const WaitAndBus& lhs, const WaitAndBus& rhs) {
	return { lhs.wait + rhs.wait,lhs.movement + rhs.movement };
}

class TransportRouter {
public:
	TransportRouter(TransportCatalogue& catalogue, int wait_time, int velocity);

	const graph::DirectedWeightedGraph<WaitAndBus>& GetGraph() { return graph_; }

	void BuildRoute(std::string_view from, std::string_view to) {
		auto result = router_.BuildRoute(stops_name_and_id_.at(from), stops_name_and_id_.at(to));
	}

private:

	double CalculateMoveWeight(uint64_t dist) {
		return (static_cast<double>(60) * dist) / (1000 * bus_velocity_);
	}

	int bus_wait_time_ = 0;
	int bus_velocity_ = 0;
	std::unordered_map<std::string_view, int, transport_catalogue::Hasher> stops_name_and_id_;
	WaitAndBus weight_;
	graph::DirectedWeightedGraph<WaitAndBus> graph_;
	graph::Router<WaitAndBus> router_{ graph_ };
};
