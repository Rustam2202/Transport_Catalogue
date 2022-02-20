// чтение запросов на заполнение базы

#pragma once

#include "transport_catalogue.h"

#include <algorithm>
#include <iostream>
#include <string_view>

using namespace std;

void DeleteBourderSpaces(string_view str) {
	str.remove_prefix(std::min(str.find_first_not_of(" "), str.size()));
	str.remove_suffix(std::max(str.find_last_not_of(" "), str.size()));
}

Stop ReadInputStop(istream& query) {
	Stop result;
	std::string stop;

	getline(query, stop, ':');
	stop.erase(stop.begin());
	stop.erase(stop.end() - 1);
	result.stop = stop;

	cin >> result.coodinates.lat;
	cin.get();
	cin >> result.coodinates.lng;

	return result;
}

Bus ReadInputBus(TransportCatalogue trans_cat, string_view str) {
	Bus result;
	auto ch = str.begin();

	// найти номер
	string num;
	while (*ch != ':')
	{
		num.push_back(*ch);
		ch++;
	}
	result.bus = stoi(num);
	ch++;
	ch++;

	// найти остановку в stops_ и вставить указатель в result
	_String_view_iterator ch_begin = ch;
	_String_view_iterator ch_end = ch;
	while (*ch != '\n') {
		if (*ch == '>' || *ch == '-' || *ch == '\n') {
			if (*ch == '-') {
				result.IsRing = true;
			}
			ch_end = ch;
			string_view stop(ch_begin, ch_end - 1);
			result.stops.insert(trans_cat.FindStop(stop));
			ch_begin = ch;
		}
		ch++;
	}

	return result;
}

void InputReader(TransportCatalogue& trans_cat) {
	int query_count;
	//vector<string> stop_queries;
	vector<string> bus_queries;

	cout << "Enter count of queries to input data: ";
	cin >> query_count;

	int query_numb = 0;
	while (query_numb < query_count) {
		string query_type;
		cin >> query_type;
		if (query_type == "Stop")
		{
			trans_cat.AddStop(ReadInputStop(cin));
			query_numb++;
		}
		else if (query_type == "Bus") {
			string temp;
			cin.get();
			getline(cin, temp);
			bus_queries.push_back(temp);
			query_numb++;
		}
		else {
			cout << "Incorrected query type (enter Stop or Bus):";
		}
	}

	for (string query : bus_queries) {
		trans_cat.AddBus(ReadInputBus(trans_cat, query));
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

*/