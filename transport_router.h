#pragma once

#include "domain.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

#include <vector>
#include <unordered_map>

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

WaitAndBus operator+(const WaitAndBus& lhs, const WaitAndBus& rhs) {
	return { lhs.wait + rhs.wait,lhs.movement + rhs.movement };
}

class TransportRouter {
public:
	TransportRouter(TransportCatalogue& catalogue, int wait_time, int velocity) :
		graph_(catalogue.GetStops().size()),
		bus_wait_time_(wait_time),
		bus_velocity_(velocity)
	{
		size_t i = 0;
		for (const auto& stop : catalogue.GetStops()) {
			stops_name_and_id_[stop.stop_name] = i;
			i++;
		}
		for (const Bus& bus : catalogue.GetBuses()) {
			for (size_t k = 0; k < bus.stops_vector.size(); ++k) {
				double route_length = 0;
				for (size_t j = k + 1; j < bus.stops_vector.size(); ++j) {
					graph::Edge<WaitAndBus> temp;
					temp.from = stops_name_and_id_.at(bus.stops_vector[k]->stop_name);
					temp.to = stops_name_and_id_.at(bus.stops_vector[j]->stop_name);
					uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[j - 1]->stop_name, bus.stops_vector[j]->stop_name);
					route_length += dist;
					temp.weight.movement = CalculateMoveWeight(route_length);
					temp.weight.wait = bus_wait_time_;
					graph_.AddEdge(temp);
				}
			}
			if (!bus.is_ring) {
				for (int k = bus.stops_vector.size() - 1; k > -1; --k) {
					double route_length = 0;
					for (int j = k - 1; j > -1; --j) {
						graph::Edge<WaitAndBus> temp;
						temp.from = stops_name_and_id_.at(bus.stops_vector[k]->stop_name);
						temp.to = stops_name_and_id_.at(bus.stops_vector[j]->stop_name);
						uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[j + 1]->stop_name, bus.stops_vector[j]->stop_name);
						route_length += dist;
						temp.weight.movement = CalculateMoveWeight(route_length);
						temp.weight.wait = bus_wait_time_;
						graph_.AddEdge(temp);
					}
				}
			}
		}
	}

	const graph::DirectedWeightedGraph<WaitAndBus>& GetGraph() { return graph_; }

	void BuildRoute(string_view from, string_view to) {
		auto result = router_.BuildRoute(stops_name_and_id_.at(from), stops_name_and_id_.at(to));
	}

private:

	double CalculateMoveWeight(uint64_t dist) {
		return (static_cast<double>(60) * dist) / (1000 * bus_velocity_);
	}

	int bus_wait_time_ = 0;
	int bus_velocity_ = 0;
	std::unordered_map<string_view, int, transport_catalogue::Hasher> stops_name_and_id_;
	WaitAndBus weight_;
	graph::DirectedWeightedGraph<WaitAndBus> graph_;
	graph::Router<WaitAndBus> router_{ graph_ };
};
