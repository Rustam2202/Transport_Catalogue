#include "stat_reader.h"

#include <iostream>

ostream& operator<<(ostream& os, const BusInfo& bus_info) {
	using namespace std;

	if (bus_info.stops_on_route == 0)
		os << "Bus " << bus_info.bus << ": not found";
	else {
		os << "Bus " << bus_info.bus << ": ";
		os << bus_info.stops_on_route << " stops on route, ";
		os << bus_info.unique_stops << " unique stops, ";
		os << bus_info.route_length << " route length";
	}
	return os;
}

ostream& operator<<(ostream& os, const StopInfo& stop_info) {
	using namespace std;

	if (stop_info.IsInStops == false) {
		os << "Stop " << stop_info.stop_name << ": not found";
	}
	else if (stop_info.stop_with_buses.size() == 0) {
		os << "Stop " << stop_info.stop_name << ": no buses";
	}
	else {
		os << "Stop " << stop_info.stop_name << ": buses ";
		auto finded = stop_info.stop_with_buses;
		for (auto bus : finded) {
			os << bus << " ";
		}
	}

	return os;
}

void OutputReader(TransportCatalogue& trans_cat) {
	using namespace std;

	vector<string> bus_queries;
	vector<string> stop_queries;
	int query_count;
	cin >> query_count;
	cin.get();

	for (int i = 0; i < query_count; ++i) {
		string query_type;
		cin >> query_type;
		cin.get();
		string temp;
		getline(cin, temp);
		if (query_type == "Stop") {
			//stop_queries.push_back(temp);
			cout << trans_cat.GetStopInfo(temp) << endl;

		}
		else if (query_type == "Bus") {
			//bus_queries.push_back(temp);
			cout << trans_cat.GetBusInfo(temp) << endl;

		}
	}
	/*for (string& bus : bus_queries) {
		cout << trans_cat.GetBusInfo(bus) << endl;
	}
	for (string& stop : stop_queries) {
		cout << trans_cat.GetStopInfo(stop) << endl;
	}*/
}