#include <transport_catalogue.pb.h>

#include "json_reader.h"

#include <fstream>
#include <iostream>
#include <ostream>

using namespace std;

void Serialization(std::istream &strm = cin)
{
	auto [catalogue, file_name] = MakeBase(strm);
	TC_Proto::TransportCatalogue tc;
	for (const auto &bus : catalogue.GetBusInfo())
	{
		TC_Proto::BusInfo bus_info;
		bus_info.set_bus_name(bus.second.bus_name.data());
		bus_info.set_curvature(bus.second.curvature);
		bus_info.set_route_length(bus.second.route_length);
		bus_info.set_stops_on_route(bus.second.stops_on_route);
		bus_info.set_unique_stops(bus.second.unique_stops);
		tc.add_buses()->CopyFrom(bus_info);
	}

	for (const auto &stop : catalogue.GetStopInfo())
	{
		TC_Proto::StopInfo stop_info;
		stop_info.set_stop_name(stop.first->stop_name.data());
		for (const auto &bus : stop.second)
		{
			stop_info.add_bus_name(bus->bus_name.data());
		}
		tc.add_stops()->CopyFrom(stop_info);
	}

	ofstream ostrm;
	ostrm.open(file_name);
	tc.SerializePartialToOstream(&ostrm);
}