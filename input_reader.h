// чтение запросов на заполнение базы

#pragma once

#include "transport_catalogue.h"

#include <iostream>

using namespace std;

enum class QueryType {
	Bus,
	Stop
};

struct Query {
	QueryType type;
	string bus;
	string stop;
	vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {
	string str;
	is >> str;
	if (str == "Bus") {
		q.type = QueryType::Bus;
		is >> q.bus;
		int stop_count;
		is >> stop_count;
		q.stops.resize(stop_count);
		for (int i = 0; i < stop_count; ++i)
			is >> q.stops[i];
	}
	else if (str == "Stop") {
		q.type = QueryType::Stop;
		is >> q.stop;
	}
	return is;
}

TransportCatalogue::Stop ReadInputStop(istream& query) {
	TransportCatalogue::Stop result;
	std::string stop;
	/*for (char ch : query) {
		if (ch == ':') {
			break;
		}
		stop.push_back(ch);
	}
	result.stop = stop;*/

	getline(query, stop, ':');

	cin.get();

	return result;
}

void InputReader(TransportCatalogue& trans_cat) {
	int query_count;
	cout << "Enter count of queries to input data: ";
	cin >> query_count;

	int query_numb = 0;
	while (query_numb < query_count) {
		string query_type;
		cin >> query_type;
		if (query_type == "Stop")
		{

			query_numb++;
		}
		else if (query_type == "Bus") {

			query_numb++;
		}
		else {
			cout << "Incorrected query type (enter Stop or Bus):";
		}
	}

}

//10
// 
//Stop Tolstopaltsevo : 55.611087, 37.208290
//Stop Marushkino : 55.595884, 37.209755
//Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
//Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
//Stop Rasskazovka : 55.632761, 37.333324
//Stop Biryulyovo Zapadnoye : 55.574371, 37.651700
//Stop Biryusinka : 55.581065, 37.648390
//Stop Universam : 55.587655, 37.645687
//Stop Biryulyovo Tovarnaya : 55.592028, 37.653656
//Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164