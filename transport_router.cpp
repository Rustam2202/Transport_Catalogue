#include "transport_router.h"

TransportGraph::TransportGraph(TransportCatalogue& catalogue, int wait_time, int velocity) :
	graph_(catalogue.GetStops().size()),
	bus_wait_time_(wait_time),
	bus_velocity_(velocity)
	//router_(graph_)
{
	size_t i = 0;
	for (const auto& stop : catalogue.GetStops()) {
		stops_name_to_id_[stop.stop_name] = i;
		i++;
	}
	for (const Bus& bus : catalogue.GetBuses()) {
		for (size_t k = 0; k < bus.stops_vector.size(); ++k) {
			double route_length = 0;
			for (size_t j = k + 1; j < bus.stops_vector.size(); ++j) {
				graph::Edge<WaitAndMovement> temp;
				temp.from = stops_name_to_id_.at(bus.stops_vector[k]->stop_name);
				temp.to = stops_name_to_id_.at(bus.stops_vector[j]->stop_name);
				uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[j - 1]->stop_name, bus.stops_vector[j]->stop_name);
				route_length += dist;
				temp.weight.movement = CalculateMoveWeight(route_length);
				temp.weight.wait = bus_wait_time_;
				temp.span_count = j - k;
				graph_.AddEdge(temp);
				id_to_bus_name_.push_back(bus.bus_name);
			}
		}
		if (!bus.is_ring) {
			for (int k = bus.stops_vector.size() - 1; k > -1; --k) {
				double route_length = 0;
				for (int j = k - 1; j > -1; --j) {
					graph::Edge<WaitAndMovement> temp;
					temp.from = stops_name_to_id_.at(bus.stops_vector[k]->stop_name);
					temp.to = stops_name_to_id_.at(bus.stops_vector[j]->stop_name);
					uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[j + 1]->stop_name, bus.stops_vector[j]->stop_name);
					route_length += dist;
					temp.weight.movement = CalculateMoveWeight(route_length);
					temp.weight.wait = bus_wait_time_;
					temp.span_count = k - j;
					graph_.AddEdge(temp);
					id_to_bus_name_.push_back(bus.bus_name);
				}
			}
		}
	}
}
