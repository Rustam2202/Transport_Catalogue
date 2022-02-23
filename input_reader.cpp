
#include "input_reader.h"

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
	while (*ch != ':')
	{
		result.bus.push_back(*ch);
		ch++;
	}

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

	for (string query : bus_queries) {
		trans_cat.AddBus(ReadInputBus(trans_cat, query));
	}

}