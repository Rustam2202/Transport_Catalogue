// чтение запросов на вывод и сам вывод

#pragma once

#include "transport_catalogue.h"

#include <iostream>

using namespace std;

ostream& operator<<(ostream& os, const BusInfo& bus_info) {
	if (bus_info.stops_on_route == 0)
		os << bus_info.bus << " : not found";
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
	cout << "Enter count of count to show buses: " << endl;
	cin >> query_count;
	cout << "Entered " << query_count << endl;

	string bus;
	vector<int> bus_queries(query_count);
	for (int i = 0; i < query_count; ++i) {
		cout << "Enter bus number: " << endl;
		cin >> bus;
		cin >> bus_queries[i];
	}

	for (int bus : bus_queries) {
		auto finded = trans_cat.GetBusInfo(bus);
		if (finded.stops_on_route > 0) {
			cout << finded << endl;
		}
		else {
			cout << "Bus " << bus << ": not found" << endl;
		}
	}
}

/*
10
Stop Tolstopaltsevo : 55.611087, 37.208290
Stop Marushkino : 55.595884, 37.209755
Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka : 55.632761, 37.333324
Stop Biryulyovo Zapadnoye : 55.574371, 37.651700
Stop Biryusinka : 55.581065, 37.648390
Stop Universam : 55.587655, 37.645687
Stop Biryulyovo Tovarnaya : 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751

Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750 : 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751 : not found
*/