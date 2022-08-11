#include <transport_catalogue.pb.h>

#include "json_reader.h"

#include <fstream>
#include <iostream>

using namespace std;

void Serialization(std::istream& strm = cin) {
	TransportCatalogue catalogue = MakeBase(strm);
	TC_Proto::TransportCatalogue tc;
	for (const auto& bus : catalogue.GetBusInfo()) {
		TC_Proto::BusInfo bus_info;
		bus_info.set_bus_name(bus.second.bus_name);
		bus_info.set_curvature(bus.second.curvature);
		bus_info.set_route_length(bus.second.route_length);
		bus_info.set_stops_on_route(bus.second.stops_on_route);
		bus_info.set_unique_stops(bus.second.unique_stops);
		tc.add_buses()->CopyFrom(bus_info);
	}

	for (const auto& stop : catalogue.GetStopInfo()) {
		TC_Proto::StopInfo stop_info;
		stop_info.set_stop_name(stop.first->stop_name);
	}
}