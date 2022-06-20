
#include "graph.h"
#include "transport_catalogue.h"

#include <unordered_map>

class TransportRouter {
public:
	TransportRouter(TransportCatalogue& catalogue) :graph_(catalogue.GetStops().size() * 2) {
		size_t i = 0;
		for (const auto& stop : catalogue.GetStops()) {
			stops_name_and_id_[stop.stop_name] = i;
			stops_id_and_name_[i] = stop.stop_name;
			i++;
		}
		for (const auto& bus : catalogue.GetBuses()) {
			for (const auto& stop : bus.stops_vector) {

			}
		}
	}

	void SetBusWaitTime(int time) {
		bus_wait_time_ = time;
	}
	void SetBusVelocity(int vel) {
		bus_velocity_ = vel;
	}

private:
	int bus_wait_time_ = 0;
	int bus_velocity_ = 0;
	std::unordered_map<int, string_view> stops_id_and_name_;
	std::unordered_map<string_view, int> stops_name_and_id_;
	graph::DirectedWeightedGraph<Stop> graph_;
};