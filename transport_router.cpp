#include "transport_router.h"

void TransportGraph::CalculateAndAddEdge(const Bus& bus, TransportCatalogue& catalogue,
	double& route_length, int k, int j) {
	graph::Edge<WeightInfo> temp;
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

TransportGraph::TransportGraph(TransportCatalogue& catalogue, int wait_time, int velocity) :
	bus_wait_time_(wait_time),
	bus_velocity_(velocity),
	graph_(catalogue.GetStops().size())
{
	size_t i = 0;
	for (const auto& stop : catalogue.GetStops()) {
		stops_name_to_id_[stop.stop_name] = i;
		i++;
	}
	for (const Bus& bus : catalogue.GetBuses()) {
		for (int k = 0; k < bus.stops_vector.size(); ++k) {
			double route_length = 0;
			for (int j = k + 1; j < bus.stops_vector.size(); ++j) {
				CalculateAndAddEdge(bus, catalogue, route_length, k, j);

				/*graph::Edge<WeightInfo> temp;
				temp.from = stops_name_to_id_.at(bus.stops_vector[k]->stop_name);
				temp.to = stops_name_to_id_.at(bus.stops_vector[j]->stop_name);
				uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[j - 1]->stop_name, bus.stops_vector[j]->stop_name);
				route_length += dist;
				temp.weight.movement = CalculateMoveWeight(route_length);
				temp.weight.wait = bus_wait_time_;
				temp.span_count = j - k;
				graph_.AddEdge(temp);
				id_to_bus_name_.push_back(bus.bus_name);*/
			}
		}

		if (!bus.is_ring) {
			for (int k = 1; k < bus.stops_vector.size(); ++k) {
				double route_length = 0;
				for (int j = k - 1; j < bus.stops_vector.size(); ++j) {
					CalculateAndAddEdge(bus, catalogue, route_length, j, k);
				}
			}
			//if (!bus.is_ring) {
			//	for (int k = bus.stops_vector.size() - 1; k > -1; --k) {
			//		double route_length = 0;
			//		for (int j = k - 1; j > -1; --j) {
			//			graph::Edge<WeightInfo> temp;
			//			temp.from = stops_name_to_id_.at(bus.stops_vector[k]->stop_name);
			//			temp.to = stops_name_to_id_.at(bus.stops_vector[j]->stop_name);
			//			uint64_t dist = catalogue.GetDistanceBetweenStops(bus.stops_vector[j + 1]->stop_name, bus.stops_vector[j]->stop_name);
			//			route_length += dist;
			//			temp.weight.movement = CalculateMoveWeight(route_length);
			//			temp.weight.wait = bus_wait_time_;
			//			temp.span_count = k - j;
			//			graph_.AddEdge(temp);
			//			id_to_bus_name_.push_back(bus.bus_name);
			//		}
			//	}
			//}
		}
	}
}

Routes TransportRoter::BuildRoute(std::string_view from, std::string_view to) {
	Routes routes;
	std::optional<std::vector<RouteInfo>> result{ 0 };
	auto route = router_.BuildRoute(GetIdOfStopName(from), GetIdOfStopName(to));
	if (route == std::nullopt) {
		routes.route = std::nullopt;
		return routes;
	}
	for (size_t edge_id : route.value().edges) {
		RouteInfo wait_part;
		wait_part.stop_name = stops_.at(GetGraph().GetEdge(edge_id).from).stop_name;
		GetGraph().GetEdge(edge_id).weight.wait;
		wait_part.time = GetGraph().GetEdge(edge_id).weight.wait;
		result.value().push_back(wait_part);
		RouteInfo move_part;
		move_part.bus_name = GetBusNameById(edge_id);
		move_part.time = GetGraph().GetEdge(edge_id).weight.movement;
		move_part.span_count = GetGraph().GetEdge(edge_id).span_count;
		result.value().push_back(move_part);
	}
	routes.route = result;
	routes.weight = route.value().weight.movement + route.value().weight.wait;
	return routes;
}
