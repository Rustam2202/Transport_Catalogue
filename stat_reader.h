// чтение запросов на вывод и сам вывод

#pragma once

#include "transport_catalogue.h"

#include <iostream>

using namespace std;

ostream& operator<<(ostream& os, const BusInfo& bus_info) {
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

void OutputReader(TransportCatalogue& trans_cat) {
	int query_count;

	cin >> query_count;
	//cout << "Entered " << query_count << endl;
	/*int query_numb = 0;
	while (query_numb < query_count) {
		string query_type;
		cin >> query_type;
		if (query_type == "Bus")
		{
			int number;
			cin >> number;
			cout << trans_cat.GetBusInfo(number) << endl;
			query_numb++;
		}
	}*/

	string bus;
	vector<int> bus_queries(query_count);
	for (int i = 0; i < query_count; ++i) {
		int number;
		string bus;
		cin >> bus;
		cin >> number;
		bus_queries[i] = number;

		/*string temp;
		cin.get();
		getline(cin, temp);
		bus_queries[i] = stoi(temp.substr(4, 6));*/
	}
	for (int bus : bus_queries) {
		cout << trans_cat.GetBusInfo(bus) << endl;
	}

}
/*
10
Stop Tolstopaltsevo: 55.611087, 37.208290
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.651700
Stop Biryusinka: 55.581065, 37.648390
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751

*/