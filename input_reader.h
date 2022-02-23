// чтение запросов на заполнение базы

#pragma once

#include "transport_catalogue.h"

#include <algorithm>
#include <iostream>
#include <string_view>

using namespace std;

Stop ReadInputStop(istream& query) {
	Stop result;
	std::string stop;

	getline(query, stop, ':');
	stop.erase(stop.begin());
	result.stop = stop;

	cin >> result.coodinates.lat;
	cin.get();
	cin >> result.coodinates.lng;

	return result;
}

Bus ReadInputBus(TransportCatalogue& trans_cat, string_view str) {
	Bus result;
	auto ch = str.begin();

	// найти номер
	//string num;
	while (*ch != ':')
	{
		result.bus.push_back(*ch);
		//num.push_back(*ch);
		ch++;
	}

	//result.bus = stoi(num);
	ch++;
	ch++;

	// найти остановку в stops_ и вставить указатель в result
	auto ch_begin = ch;
	auto ch_end = ch;

	result.IsRing = any_of(ch_begin, str.end(), [](const char c) {return c == '>'; });

	while (true) {
		if (*ch == '>' || *ch == '-') {
			ch_end = ch - 1;
			string stop(ch_begin, ch_end);

			// result.stops[trans_cat.FindStop(stop)]++; // unordered_map<Stop*, size_t> stops;
			result.stops_unique.insert(trans_cat.FindStop(stop));
			result.stops_vector.push_back(trans_cat.FindStop(stop));
			ch_begin = ch + 2;
		}
		else if (next(ch) == str.end())
		{
			ch_end = str.end();
			string stop(ch_begin, ch_end);
			//	result.stops[trans_cat.FindStop(stop)]++; // unordered_map<Stop*, size_t> stops;
			result.stops_unique.insert(trans_cat.FindStop(stop));
			result.stops_vector.push_back(trans_cat.FindStop(stop));
			break;
		}

		ch++;
	}

	return result;
}

void InputReader(TransportCatalogue& trans_cat) {
	int query_count;
	vector<string> bus_queries;

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
	}
	//cin.get();

	for (string query : bus_queries) {
		trans_cat.AddBus(ReadInputBus(trans_cat, query));
	}

}

/*

4
Stop Stop1: 55.611087, 37.208290
Stop Stop2: 55.595884, 37.209755
Bus 1: Stop1 - Stop2 - Stop1
Bus 2: Stop1 > Stop2 > Stop1
2
Bus 1
Bus 2

3
Bus 222 AAA: Stop1 - Stop2
Stop Stop1: 55.574371, 37.651700
Stop Stop2: 55.581065, 37.648390
1
Bus 222 AAA

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

*/