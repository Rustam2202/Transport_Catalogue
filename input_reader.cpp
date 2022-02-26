
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

struct NameLenghts
{
	string this_stop_name;
	string lengths_data;
};

NameLenghts ReadSimpleStop(string_view str, TransportCatalogue& trans_cat) {
	auto ch = str.begin();
	string stop_name;

	// имя остановки
	while (*ch != ':')
	{
		stop_name.push_back(*ch);
		ch++;
	}
	ch++;

	// координаты
	double lat;
	double lng;
	string temp;
	while (*ch != ',') {
		temp.push_back(*ch);
		ch++;
	}

	lat = stod(temp);
	temp.clear();
	ch++;
	while (true) {
		if (*ch == ',') {
			lng = stod(temp);
			break;
		}
		else if (next(ch) == str.end()) {
			//ch++;
			temp.push_back(*ch);
			lng = stod(temp);
			ch++;
			break;
		}
		temp.push_back(*ch);
		ch++;
	}

	Stop result;
	result.stop = stop_name;
	result.coodinates.lat = lat;
	result.coodinates.lng = lng;
	trans_cat.AddStop(result);

	NameLenghts ret;
	if (ch == str.end()) {
		return ret;
	}

	ch++;
	str.remove_prefix(ch - str.begin());
	ret.this_stop_name = stop_name;
	ret.lengths_data = str;
	return ret;
}

void ReadStopLenghtsData(NameLenghts& lng_data, TransportCatalogue& trans_cat) {
	auto ch = lng_data.lengths_data.begin() + 1;
	auto ch_begin = ch;
	auto ch_end = ch;

	while (true) {
		if (*ch == 'm') {

			ch_end = ch;
			string lenght_temp(ch_begin, ch_end);
			uint64_t lenght = stoi(lenght_temp);
			ch += 5;
			ch_begin = ch;
			while (true) {
				if (*ch == ',') {
					ch_end = ch;
					string other_stop_name(ch_begin, ch_end);
					trans_cat.SetDistanceBetweenStops(lng_data.this_stop_name, other_stop_name, lenght);
					ch++;
					ch_begin = ch;
					break;
				}
				else if (next(ch) == lng_data.lengths_data.end()) {
					ch_end = ch + 1;
					string other_stop_name(ch_begin, ch_end);
					trans_cat.SetDistanceBetweenStops(lng_data.this_stop_name, other_stop_name, lenght);
					ch++;
					break;
				}
				ch++;
			}

		}
		if (ch == lng_data.lengths_data.end()) {
			return;
		}
		ch++;
	}
}

void ReadInputStop(TransportCatalogue& trans_cat, string_view str) {

	auto ch = str.begin();
	string stop_name;

	// имя остановки

	while (*ch != ':')
	{
		stop_name.push_back(*ch);
		ch++;
	}
	ch++;

	// координаты

	double lat;
	double lng;
	string temp;
	while (*ch != ',') {
		temp.push_back(*ch);
		ch++;
	}

	lat = stod(temp);
	temp.clear();
	ch++;
	while (*ch != ',') {
		temp.push_back(*ch);
		ch++;
	}
	lng = stod(temp);
	temp.clear();

	Stop result;
	result.stop = stop_name;
	result.coodinates.lat = lat;
	result.coodinates.lng = lng;
	trans_cat.AddStop(result);


	if (ch == str.end()) {
		return;
	}

	ch++;
	auto ch_begin = ch;
	auto ch_end = ch;

	while (true) {
		if (*ch == 'm') {
			ch_end = ch;
			string lenght_temp(ch_begin, ch_end);
			uint64_t lenght = stoi(lenght_temp);
			ch += 5;
			ch_begin = ch;
			while (true) {
				if (*ch == ',') {
					ch_end = ch;
					string other_stop_name(ch_begin, ch_end);
					trans_cat.SetDistanceBetweenStops(stop_name, other_stop_name, lenght);
					ch++;
					ch_begin = ch;
					break;
				}
				else if (next(ch) == str.end()) {
					ch_end = ch + 1;
					string other_stop_name(ch_begin, ch_end);
					trans_cat.SetDistanceBetweenStops(stop_name, other_stop_name, lenght);
					ch++;
					break;
				}
				ch++;
			}

		}
		if (ch == str.end()) {
			return;
		}
		ch++;
	}

}

// Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino
// Stop Prazhskaya: 55.611678, 37.603831


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
	vector<string> stop_queries;
	vector<string> bus_queries;
	vector<NameLenghts> lengths_queries;

	cin >> query_count;

	int query_numb = 0;
	while (query_numb < query_count) {
		string query_type;
		cin >> query_type;
		if (query_type == "Stop")
		{
			string temp;
			cin.get();
			getline(cin, temp);
			stop_queries.push_back(temp);
			query_numb++;
			/*trans_cat.AddStop(ReadInputStop(cin));
			query_numb++;*/
		}
		else if (query_type == "Bus") {
			string temp;
			cin.get();
			getline(cin, temp);
			bus_queries.push_back(temp);
			query_numb++;
		}
	}
	for (string query : stop_queries) {
		auto query_lengths = ReadSimpleStop(query, trans_cat);
		if (!query_lengths.lengths_data.empty()) {
			lengths_queries.push_back(query_lengths);
		}
	}
	for (auto lng_data : lengths_queries) {
		ReadStopLenghtsData(lng_data, trans_cat);
	}
	for (string query : bus_queries) {
		trans_cat.AddBus(ReadInputBus(trans_cat, query));
	}

}