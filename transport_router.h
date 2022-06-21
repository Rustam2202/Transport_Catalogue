
#include "domain.h"
#include "graph.h"
#include "transport_catalogue.h"

#include <vector>
#include <unordered_map>

struct WaitAndBus {
	double wait = 0;
	double movement = 0;
};

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
			//stops_id_and_name_[i] = stop.stop_name;
			i++;
		}
		for (const Bus& bus : catalogue.GetBuses()) {
			//size_t i = 0;
			for (size_t j = 0; j < bus.stops_vector.size() - 1; ++j) {
				for (size_t i = j; i < bus.stops_vector.size() - 1; ++i) {
					graph::Edge<WaitAndBus> temp;
					temp.from = stops_name_and_id_.at(bus.stops_vector[i]->stop_name);
					temp.to = stops_name_and_id_.at(bus.stops_vector[i + 1]->stop_name);
					uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[i]->stop_name, bus.stops_vector[i + 1]->stop_name);
					temp.weight.movement = CalculateMoveWeight(dist);
					if (i == 0 || HasTransfer(catalogue, bus.stops_vector[i]->stop_name)) {
						temp.weight.wait = bus_wait_time_;
					}
					
					graph_.AddEdge(temp);
				}
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

	double CalculateMoveWeight(uint64_t dist) {
		return (static_cast<double>(60) * dist) / (1000 * bus_velocity_);
	}

	bool HasTransfer(TransportCatalogue& catalogue, string_view this_stop) {
		const auto& stops = catalogue.GetStopInfo();
		const auto& stop = stops.at(catalogue.FindStop(this_stop));
		return stop.size() > 1;
	}

	void AddEdge(graph::Edge<WaitAndBus> item) {
		edges_.push_back(item);
	}

	int bus_wait_time_ = 0;
	int bus_velocity_ = 0;
	//std::unordered_map<int, string_view> stops_id_and_name_;
	std::unordered_map<string_view, int, transport_catalogue::Hasher> stops_name_and_id_;
	WaitAndBus weight_;
	graph::DirectedWeightedGraph<WaitAndBus> graph_;
	std::vector<graph::Edge<WaitAndBus>> edges_;
};