
#include "graph.h"
#include "transport_catalogue.h"


class TransportRouter {
public:
	TransportRouter(TransportCatalogue& catalogue) :graph_(catalogue.GetStops().size()) {

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
	graph::DirectedWeightedGraph<Stop> graph_;
};