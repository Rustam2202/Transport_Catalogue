#include "input_reader.h"

#include <algorithm>
#include <iostream>

namespace transport_catalogue {
	using namespace std;

	namespace detail {

		struct RouteLenghts
		{
			string this_stop_name;
			string lengths_data;
		};

		RouteLenghts ParsingStop(istream& input_data, TransportCatalogue& catalogue) {
			Stop result;

			// имя остановки
			getline(input_data, result.stop, ':');
			cin.get();

			// координаты
			cin >> result.coodinates.lat;
			cin.get();
			cin >> result.coodinates.lng;

			catalogue.AddStop(result);
			// запись имя+координаты

			// оставшиеся длины или пустая строка
			RouteLenghts lengths;
			lengths.this_stop_name = result.stop;
			getline(input_data, lengths.lengths_data);
			return lengths;
		}

		void ParsingStopsLength(RouteLenghts& stop_and_length, TransportCatalogue& catalogue) {

			auto ch = stop_and_length.lengths_data.begin() + 1;
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
							catalogue.SetDistanceBetweenStops(stop_and_length.this_stop_name, other_stop_name, lenght);
							ch++;
							ch_begin = ch;
							break;
						}
						else if (next(ch) == stop_and_length.lengths_data.end()) {
							ch_end = ch + 1;
							string other_stop_name(ch_begin, ch_end);
							catalogue.SetDistanceBetweenStops(stop_and_length.this_stop_name, other_stop_name, lenght);
							ch++;
							break;
						}
						ch++;
					}
				}
				if (ch == stop_and_length.lengths_data.end()) {
					return;
				}
				ch++;
			}
		}
	}

	Bus ParsingBus(TransportCatalogue& catalogue, string_view str) {
		Bus result;
		auto ch = str.begin();

		// найти номер
		auto end = str.find(':');
		result.bus = { &str[0], end };
		ch += end + 2;

		/*while (*ch != ':')
		{
			result.bus.push_back(*ch);
			ch++;
		}
		ch++;
		ch++;*/

		// найти остановку в stops_ и вставить указатель в result
		auto ch_begin = ch;
		auto ch_end = ch;

		result.is_ring = any_of(ch_begin, str.end(), [](const char c) {return c == '>'; });

		while (true) {
			if (*ch == '>' || *ch == '-') {
				ch_end = ch - 1;
				string stop(ch_begin, ch_end);
				result.stops_unique.insert(catalogue.FindStop(stop));
				result.stops_vector.push_back(catalogue.FindStop(stop));
				ch_begin = ch + 2;
			}
			else if (next(ch) == str.end())
			{
				ch_end = str.end();
				string stop(ch_begin, ch_end);
				result.stops_unique.insert(catalogue.FindStop(stop));
				result.stops_vector.push_back(catalogue.FindStop(stop));
				break;
			}
			ch++;
		}
		return result;
	}

	void InputReader(TransportCatalogue& catalogue, istream& input) {
		using namespace transport_catalogue::detail; // InputReader вне пространства имен detail
		int query_count;
		vector<string> stop_queries;
		vector<string> bus_queries;
		vector<RouteLenghts> lengths_queries;

		input >> query_count;
		int query_numb = 0;
		while (query_numb < query_count) {
			string query_type;
			input >> query_type;
			if (query_type == "Stop")
			{
				string temp;
				input.get();
				lengths_queries.push_back(ParsingStop(input, catalogue));
				query_numb++;
			}
			else if (query_type == "Bus") {
				string temp;
				input.get();
				getline(input, temp);
				bus_queries.push_back(temp);
				query_numb++;
			}
		}

		for (RouteLenghts stop_and_length : lengths_queries) {
			if (!stop_and_length.lengths_data.empty()) {
				ParsingStopsLength(stop_and_length, catalogue);
			}
		}
		for (string query : bus_queries) {
			catalogue.AddBus(ParsingBus(catalogue, query));
		}
	}
}