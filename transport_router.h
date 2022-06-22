
#include "domain.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

#include <vector>
#include <unordered_map>

namespace transport_catalogue {
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
				//stops_id_and_name_[i] = stop.stop_name;
				i++;
			}

			for (const Bus& bus : catalogue.GetBuses()) {
				for (size_t rout_stop_num = 0; rout_stop_num < bus.stops_vector.size(); ++rout_stop_num) {
					for (size_t rout_stop_num_incr = rout_stop_num + 1; rout_stop_num_incr < bus.stops_vector.size(); ++rout_stop_num_incr) {
						graph::Edge<WaitAndBus> temp;
						temp.from = stops_name_and_id_.at(bus.stops_vector[rout_stop_num]->stop_name);
						temp.to = stops_name_and_id_.at(bus.stops_vector[rout_stop_num_incr]->stop_name);
						uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[rout_stop_num_incr - 1]->stop_name, bus.stops_vector[rout_stop_num_incr]->stop_name);
						temp.weight.movement = CalculateMoveWeight(dist);
						if (graph_.GetEdgeCount()) {
							temp.weight.movement += graph_.GetEdge(graph_.GetEdgeCount() - 1).weight.movement;
						}
						temp.weight.wait = bus_wait_time_;
						graph_.AddEdge(temp);
					}
				}
				if (bus.is_ring) {
					for (int k = bus.stops_vector.size() - 1; k > -1; --k) {
						for (int j = k - 1; j > -1; --j) {
							graph::Edge<WaitAndBus> temp;
							temp.from = stops_name_and_id_.at(bus.stops_vector[k]->stop_name);
							temp.to = stops_name_and_id_.at(bus.stops_vector[j]->stop_name);
							uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[j + 1]->stop_name, bus.stops_vector[j]->stop_name);
							temp.weight.movement = CalculateMoveWeight(dist);
							if (graph_.GetEdgeCount()) {
								temp.weight.movement += graph_.GetEdge(graph_.GetEdgeCount() - 1).weight.movement;
							}
							temp.weight.wait = bus_wait_time_;
							graph_.AddEdge(temp);
						}
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

		const graph::DirectedWeightedGraph<WaitAndBus>& GetGraph() {
			return graph_;
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

		int bus_wait_time_ = 0;
		int bus_velocity_ = 0;
		std::unordered_map<string_view, int, transport_catalogue::Hasher> stops_name_and_id_;
		WaitAndBus weight_;
		graph::DirectedWeightedGraph<WaitAndBus> graph_;
	};
}