#include <transport_catalogue.pb.h>

#include "json_reader.h"

#include <fstream>
#include <iostream>
#include <ostream>

using namespace std;

void Serialization(std::istream& strm = cin) {
	auto [catalogue, file_name] = MakeBase(strm);
	TC_Proto::TransportCatalogue tc;
	for (const auto& bus : catalogue.GetBusInfo())
	{
		TC_Proto::BusInfo bus_info;
		bus_info.set_bus_name(bus.second.bus_name.data());
		bus_info.set_curvature(bus.second.curvature);
		bus_info.set_route_length_on_road(bus.second.route_length_on_road);
		bus_info.set_stops_on_route(bus.second.stops_on_route);
		bus_info.set_unique_stops(bus.second.unique_stops);
		tc.add_buses()->CopyFrom(bus_info);
	}

	for (const auto& stop : catalogue.GetStopInfo())
	{
		TC_Proto::StopInfo stop_info;
		stop_info.set_stop_name(stop.first->stop_name.data());
		for (const auto& bus : stop.second)
		{
			stop_info.add_bus_name(bus->bus_name.data());
		}
		tc.add_stops()->CopyFrom(stop_info);
	}

	ofstream ostrm;
	ostrm.open(file_name, ios::binary);
	tc.SerializeToOstream(&ostrm);
	ostrm.close();
}

void DeSerialization(std::istream& strm = std::cin, std::ostream& output = std::cout) {
	TC_Proto::TransportCatalogue tc;
	json::Node base = Load(strm).GetRoot();
	string file_name = base.AsDict().at("serialization_settings").AsDict().at("file").AsString();
	const auto& requests = base.AsDict().at("stat_requests");
	ifstream data_base_file(file_name, ios::binary);

	tc.ParseFromIstream(&data_base_file);

	Builder result;
	result.StartArray();
	for (const auto& stat_data : base.AsDict().at("stat_requests").AsArray()) {
		if (stat_data.AsDict().at("type").AsString() == "Stop") {
			size_t i = 0;
			bool founded = false;

			for (auto it = tc.stops().begin(); it != tc.stops().end(); ++it, ++i) {
				if (stat_data.AsDict().at("name").AsString() == (*it).stop_name()) {
					founded = true;
					break;
				}
			}

			//for (i; i < tc.stops().size(); ++i) {
			//	if (stat_data.AsDict().at("name").AsString() == tc.stops().Get(i).stop_name()) {
			//		founded = true;
			//		break;
			//	}
			//}

			if (founded == false) {
				result.StartDict()
					.Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt())
					.Key("error_message"s).Value("not found"s)
					.EndDict();
			}
			else {
				Array buses_arr;
				if (tc.buses_size() > 0) {
					for (const auto& bus : tc.buses()) {
						buses_arr.push_back(bus.bus_name());
					}
					std::sort(buses_arr.begin(), buses_arr.end(),
						[](const Node& lhs, const Node& rhs) {return lhs.AsString() < rhs.AsString(); });
				}
				result.StartDict()
					.Key("buses"s).Value(buses_arr)
					.Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt());
					

				/*result.StartDict().Key("buses").StartArray();
			for (auto it = tc.stops().Get(i).bus_name().begin(); it != tc.stops().Get(i).bus_name().end(); ++it) {
				result.Value(*it);
			}*/
				result.Key("request_id").Value(stat_data.AsDict().at("id").AsInt()).EndDict();
			}
		}
		else if (stat_data.AsDict().at("type").AsString() == "Bus") {
			size_t i = 0;
			bool founded = false;

			for (auto it = tc.buses().begin(); it != tc.buses().end(); ++it, ++i) {
				if (stat_data.AsDict().at("name").AsString() == (*it).bus_name()) {
					founded = true;
					break;
				}
			}

			//for (i; i < tc.stops().size(); ++i) {
			//	if (stat_data.AsDict().at("name").AsString() == tc.buses().Get(i).bus_name()) {
			//		founded = true;
			//		break;
			//	}
			//}

			if (founded == false) {
				result.StartDict()
					.Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt())
					.Key("error_message").Value("not found"s)
					.EndDict();
			}
			else {
				result.StartDict()
					.Key("curvature").Value(tc.buses().Get(i).curvature())
					.Key("request_id").Value(stat_data.AsDict().at("id").AsInt())
					.Key("route_length").Value(static_cast<int>(tc.buses().Get(i).route_length_on_road()))
					.Key("stop_count").Value(static_cast<int>(tc.buses().Get(i).stops_on_route()))
					.Key("unique_stop_count").Value(static_cast<int>(tc.buses().Get(i).unique_stops()))
					.EndDict();
			}
		}
	}
	result.EndArray();
	json::Print(Document(result.Build()), output);
}