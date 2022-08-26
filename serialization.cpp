#include "serialization.h"
#include "router.h"

#include <google/protobuf/util/json_util.h>

#include <iomanip>
#include <sstream>

MR_Proto::RenderSettings MakeRenderSettings(Node base);

void SerializationBusesAndStopsInfo(transport_catalogue::TransportCatalogue& catalogue, TC_Proto::TransportCatalogue& tc);

void SerializationRouterData(TransportRoter& router, TC_Proto::TransportCatalogue& tc);

void MessageToJson(string& answer, google::protobuf::Message* m) {
	string answer_str;
	google::protobuf::util::JsonPrintOptions opt;
	opt.add_whitespace = true;
	opt.preserve_proto_field_names = true;
	opt.always_print_primitive_fields = true;
	google::protobuf::util::MessageToJsonString(*m, &answer, opt);
	answer.push_back(',');
	//istringstream iss(answer_str);
	//output << iss.str() << ',';
}

void Serialization(std::istream& strm) {
	Node base = Load(strm).GetRoot();
	TransportCatalogue catalogue;
	MapRenderer map;
	string file_name = base.AsDict().at("serialization_settings").AsDict().at("file").AsString();

	InsertStops(catalogue, base.AsDict().at("base_requests").AsArray());
	InsertStopsDistances(catalogue, base.AsDict().at("base_requests").AsArray());
	InsertBuses(catalogue, base.AsDict().at("base_requests").AsArray());
	SetMapRender(map, base.AsDict().at("render_settings"));
	TransportRoter router(
		catalogue,
		base.AsDict().at("routing_settings").AsDict().at("bus_wait_time").AsInt(),
		base.AsDict().at("routing_settings").AsDict().at("bus_velocity").AsInt()
	);

	TC_Proto::TransportCatalogue tc;
	SerializationBusesAndStopsInfo(catalogue, tc);
	tc.mutable_render_settings()->CopyFrom(MakeRenderSettings(base.AsDict().at("render_settings")));
	SerializationRouterData(router, tc);

	ofstream ostrm;
	ostrm.open(file_name, ios::binary);
	tc.SerializeToOstream(&ostrm);
}

void DeSerialization(std::istream& strm, std::ostream& output) {
	TC_Proto::TransportCatalogue tc;
	json::Node base = Load(strm).GetRoot();
	string file_name = base.AsDict().at("serialization_settings").AsDict().at("file").AsString();
	const auto& requests = base.AsDict().at("stat_requests");
	ifstream data_base_file(file_name, ios::binary);
	tc.ParseFromIstream(&data_base_file);
	MapRenderer render;
	std::vector<geo::Coordinates> coords;
	string answer;
	answer.push_back('[');

	//Builder result;
	//result.StartArray();
	for (const auto& stat_data : base.AsDict().at("stat_requests").AsArray()) {
		if (stat_data.AsDict().at("type").AsString() == "Stop") {
			const std::string& stop_name = stat_data.AsDict().at("name").AsString();
			auto it = tc.stops_info().begin();
			while (it != tc.stops_info().end()) {
				if ((*it).stop_name() == stop_name) {
					break;
				}
				++it;
			}
			if (it == tc.stops_info().end()) {
				TC_Proto::NotFound not_found;
				not_found.set_request_id(stat_data.AsDict().at("id").AsInt());
				not_found.set_error_message("not found"s);
				MessageToJson(answer, &not_found);
			}
			else {
				Array buses_arr;
				if (tc.buses_info_size() > 0) {
					for (uint32_t bus_index : (*it).bus_names_indexes()) {
						buses_arr.push_back(tc.buses_info().Get(bus_index).bus_name());
					}
					std::sort(buses_arr.begin(), buses_arr.end(),
						[](const Node& lhs, const Node& rhs) {return lhs.AsString() < rhs.AsString(); });
				}
				TC_Proto::StopWithBuses s;
				for (const auto& bus : buses_arr) {
					s.add_buses(bus.AsString());
				}
				s.set_request_id(stat_data.AsDict().at("id").AsInt());
				MessageToJson(answer, &s);
			}
		}
		else if (stat_data.AsDict().at("type").AsString() == "Bus") {
			size_t i = 0;
			bool founded = false;
			TC_Proto::Dict dict;
			TC_Proto::ValueType val;

			for (auto it = tc.buses_info().begin(); it != tc.buses_info().end(); ++it, ++i) {
				if (stat_data.AsDict().at("name").AsString() == (*it).bus_name()) {
					founded = true;
					break;
				}
			}
			if (founded == false) {
				TC_Proto::NotFound not_found;
				not_found.set_request_id(stat_data.AsDict().at("id").AsInt());
				not_found.set_error_message("not found"s);
				MessageToJson(answer, &not_found);
			}
			else {
				TC_Proto::BusInfoFinded b;
				b.set_curvature(tc.buses_info().Get(i).curvature());
				b.set_request_id(stat_data.AsDict().at("id").AsInt());
				b.set_route_length(static_cast<double>(tc.buses_info().Get(i).route_length_on_road()));
				b.set_stop_count(static_cast<int>(tc.buses_info().Get(i).stops_on_route()));
				b.set_unique_stop_count(static_cast<int>(tc.buses_info().Get(i).unique_stops()));
				MessageToJson(answer, &b);
			}
		}
		else if (stat_data.AsDict().at("type").AsString() == "Map") {
			render.SetRender(tc.render_settings().width(), tc.render_settings().height(), tc.render_settings().padding()
				, tc.render_settings().stop_radius(), tc.render_settings().line_width(), tc.render_settings().underlayer_width()
				, tc.render_settings().bus_label_font_size(), tc.render_settings().stop_label_font_size()
				, tc.render_settings().bus_label_offset().x(), tc.render_settings().bus_label_offset().y()
				, tc.render_settings().stop_label_offset().x(), tc.render_settings().stop_label_offset().y()
			);

			if (tc.render_settings().underlayer_color().has_color_rgb()) {
				render.SetUnderlayerColor(tc.render_settings().underlayer_color().color_rgb().red(),
					tc.render_settings().underlayer_color().color_rgb().green(),
					tc.render_settings().underlayer_color().color_rgb().blue()
				);
			}
			else if (tc.render_settings().underlayer_color().has_color_rgba()) {
				render.SetUnderlayerColor(tc.render_settings().underlayer_color().color_rgba().red(),
					tc.render_settings().underlayer_color().color_rgba().green(),
					tc.render_settings().underlayer_color().color_rgba().blue(),
					tc.render_settings().underlayer_color().color_rgba().opacity()
				);
			}
			else {
				render.SetUnderlayerColor(tc.render_settings().underlayer_color().color_str());
			}

			int i = 0;
			for (auto it = tc.render_settings().color_palette().begin(); it != tc.render_settings().color_palette().end(); ++it, ++i) {
				if ((*it).has_color_rgb()) {
					render.SetColorPalette(
						tc.render_settings().color_palette().Get(i).color_rgb().red(),
						tc.render_settings().color_palette().Get(i).color_rgb().green(),
						tc.render_settings().color_palette().Get(i).color_rgb().blue()
					);
				}
				else if ((*it).has_color_rgba()) {
					render.SetColorPalette(
						tc.render_settings().color_palette().Get(i).color_rgba().red(),
						tc.render_settings().color_palette().Get(i).color_rgba().green(),
						tc.render_settings().color_palette().Get(i).color_rgba().blue(),
						tc.render_settings().color_palette().Get(i).color_rgba().opacity()
					);
				}
				else {
					render.SetColorPalette(tc.render_settings().color_palette().Get(i).color_str());
				}
			}

			for (const auto& stop : tc.stops_info()) {
				if (stop.bus_names_indexes_size() > 0) {
					coords.push_back({ stop.lat(),stop.lng() });
				}
			}
			render.MakeSphereProjector(coords);

			for (const auto& bus : tc.buses_info()) {
				if (bus.stops_indexes_size() > 0) {
					for (uint32_t stop_index : bus.stops_indexes()) {
						render.AddBusWithStops(bus.bus_name(), bus.is_ring(), tc.stops_info().Get(stop_index).stop_name()
							, { tc.stops_info().Get(stop_index).lat(),tc.stops_info().Get(stop_index).lng() });
					}
				}
			}

			render.RenderBusesLines();
			render.Sorting();
			render.RenderBusesNames();
			render.RenderCircle();
			render.RenderStopsNames();
			std::stringstream s;
			render.Rendering(s);

			TC_Proto::Map m;
			m.set_map(s.str());
			m.set_request_id(stat_data.AsDict().at("id").AsInt());
			MessageToJson(answer, &m);
		}
		else if (stat_data.AsDict().at("type").AsString() == "Route") {
			const std::string& stop_from = stat_data.AsDict().at("from").AsString();
			const std::string& stop_to = stat_data.AsDict().at("to").AsString();
			uint64_t index_from = 0;
			uint64_t index_to = 0;
			bool finded_from = false;
			bool finded_to = false;

			for (const auto& stop : tc.stops_info()) {
				if (stop.stop_name().data() == stop_from) {
					index_from = stop.index();
					finded_from = true;
				}
				if (stop.stop_name().data() == stop_to) {
					index_to = stop.index();
					finded_to = true;
				}
				if (finded_from && finded_to) {
					break;
				}
			}


			const Router_proto::Route& route_internal_data = tc.router().routes_internal_data().Get(index_from).stops_to().Get(index_to);
			if (route_internal_data.is_null()) {
				TC_Proto::NotFound not_found;
				not_found.set_request_id(stat_data.AsDict().at("id").AsInt());
				not_found.set_error_message("not found"s);
				MessageToJson(answer, &not_found);
				//result.StartDict().Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt()).Key("error_message"s).Value("not found"s).EndDict();
			}
			else {
				double total_time = route_internal_data.internal_data().weight().movement() + route_internal_data.internal_data().weight().wait();
				std::vector<uint64_t> edges;
				std::optional<uint64_t> edge_id;
				if (route_internal_data.internal_data().prev_edge_null()) {
					edge_id = nullopt;
				}
				else {
					edge_id = route_internal_data.internal_data().prev_edge();
				}
				while (edge_id != nullopt) {
					edges.push_back(*edge_id);
					if (!tc.router().routes_internal_data().Get(index_from).stops_to().Get(tc.router().edges().Get(*edge_id).from()).internal_data().prev_edge_null()) {
						edge_id = tc.router().routes_internal_data().Get(index_from).stops_to().Get(tc.router().edges().Get(*edge_id).from()).internal_data().prev_edge();
					}
					else {
						edge_id = nullopt;
					}
				}
				std::reverse(edges.begin(), edges.end());

				/*	TC_Proto::Dict items;
					TC_Proto::Array items_array;
					TC_Proto::ValueType val;*/
					//result.StartDict().Key("items").StartArray();

				TC_Proto::RouteInfo r;
				for (auto edge : edges) {
					TC_Proto::RouteInfo::Item item_stop;
					item_stop.set_stop_name(tc.stops_info().Get(tc.router().edges().Get(edge).from()).stop_name());
					item_stop.set_time(tc.router().edges().Get(edge).weight().wait());
					item_stop.set_type("Wait"s);
					r.add_items()->CopyFrom(item_stop);
					TC_Proto::RouteInfo::Item item_bus;
					item_bus.set_bus(tc.buses_info().Get(tc.router().edges().Get(edge).bus_name_index()).bus_name());
					item_bus.set_time(static_cast<double>(tc.router().edges().Get(edge).weight().movement()));
					item_bus.set_type("Bus"s);
					item_bus.set_span_count(tc.router().edges().Get(edge).span_count());
					r.add_items()->CopyFrom(item_bus);

					//TC_Proto::Dict dict_to_items;

					//val.set_as_string(tc.stops_info().Get(tc.router().edges().Get(edge).from()).stop_name());
					//dict_to_items.mutable_data()->insert({ "stop_name"s ,val });
					//val.set_as_double(tc.router().edges().Get(edge).weight().wait());
					//dict_to_items.mutable_data()->insert({ "time"s ,val });
					//val.set_as_string("Wait"s);
					//dict_to_items.mutable_data()->insert({ "type"s ,val });
					//items_array.add_values()->mutable_asdict()->CopyFrom(dict_to_items);
					////		->set_allocated_asdict(&dict_to_items);
					//dict_to_items.clear_data();

					//val.set_as_string(tc.buses_info().Get(tc.router().edges().Get(edge).bus_name_index()).bus_name());
					//dict_to_items.mutable_data()->insert({ "bus"s ,val });
					//val.set_as_int(tc.router().edges().Get(edge).span_count());
					//dict_to_items.mutable_data()->insert({ "span_count"s ,val });
					//val.set_as_double(tc.router().edges().Get(edge).weight().movement());
					//dict_to_items.mutable_data()->insert({ "time"s ,val });
					//val.set_as_string("Bus"s);
					//dict_to_items.mutable_data()->insert({ "type"s ,val });

					//	results_proto.mutable_results()->add_values()->set_allocated_asdict(&dict_to_items);

						/*result.StartDict()
							.Key("stop_name").Value(tc.stops_info().Get(tc.router().edges().Get(edge).from()).stop_name())
							.Key("time").Value(tc.router().edges().Get(edge).weight().wait())
							.Key("type").Value("Wait"s)
							.EndDict();
						result.StartDict()
							.Key("bus").Value(tc.buses_info().Get(tc.router().edges().Get(edge).bus_name_index()).bus_name())
							.Key("span_count").Value(tc.router().edges().Get(edge).span_count())
							.Key("time").Value(tc.router().edges().Get(edge).weight().movement())
							.Key("type").Value("Bus"s)
							.EndDict();*/
				}
				r.set_request_id(stat_data.AsDict().at("id").AsInt());
				r.set_total_time(total_time);
				MessageToJson(answer, &r);

				/*	string answer_str;
					google::protobuf::util::JsonPrintOptions opt;
					opt.preserve_proto_field_names = true;
					opt.add_whitespace = true;
					google::protobuf::util::MessageToJsonString(r, &answer_str, opt);
					istringstream iss(answer_str);
					output << iss.str();*/

					/*	val.mutable_as_array()->CopyFrom(items_array);
						items.mutable_data()->insert({ "items" ,val });
						val.set_as_int(stat_data.AsDict().at("id").AsInt());
						items.mutable_data()->insert({ "request_id" ,val });
						val.set_as_double(total_time);
						items.mutable_data()->insert({ "total_time" ,val });*/
						//results_proto.mutable_results()->add_values()->set_allocated_asdict(&items);
					//	results_proto.mutable_results()->add_values()->mutable_asdict()->CopyFrom(items);

						/*result.EndArray();
						result.Key("request_id").Value(stat_data.AsDict().at("id").AsInt());
						result.Key("total_time").Value(total_time).EndDict();*/
			}
		}
	}

	answer.pop_back(); // delete last ','
	answer.push_back(']');
	istringstream iss(answer);
	output << iss.str();

	/*string answer_str;
	google::protobuf::util::JsonPrintOptions opt;
	opt.preserve_proto_field_names = true;
	opt.add_whitespace = true;
	google::protobuf::util::MessageToJsonString(results_proto, &answer_str, opt);
	istringstream iss(answer_str);
	output << iss.str();*/

	//results_proto.PrintDebugString();
	//result.EndArray();
	//json::Print(Document(result.Build()), output);
}

MR_Proto::RenderSettings MakeRenderSettings(Node base) {
	MR_Proto::RenderSettings settings;
	settings.set_width(base.AsDict().at("width").AsDouble());
	settings.set_height(base.AsDict().at("height").AsDouble());
	settings.set_padding(base.AsDict().at("padding").AsDouble());
	settings.set_stop_radius(base.AsDict().at("stop_radius").AsDouble());
	settings.set_line_width(base.AsDict().at("line_width").AsDouble());
	settings.set_underlayer_width(base.AsDict().at("underlayer_width").AsDouble());
	settings.set_bus_label_font_size(base.AsDict().at("bus_label_font_size").AsInt());
	settings.set_stop_label_font_size(base.AsDict().at("stop_label_font_size").AsInt());
	settings.mutable_stop_label_offset()->set_x(base.AsDict().at("stop_label_offset").AsArray()[0].AsDouble());
	settings.mutable_stop_label_offset()->set_y(base.AsDict().at("stop_label_offset").AsArray()[1].AsDouble());
	settings.mutable_bus_label_offset()->set_x(base.AsDict().at("bus_label_offset").AsArray()[0].AsDouble());
	settings.mutable_bus_label_offset()->set_y(base.AsDict().at("bus_label_offset").AsArray()[1].AsDouble());

	if (base.AsDict().at("underlayer_color").IsString()) {
		settings.mutable_underlayer_color()->set_color_str(base.AsDict().at("underlayer_color").AsString());
	}
	else if (base.AsDict().at("underlayer_color").IsArray()) {
		Node color = base.AsDict().at("underlayer_color").AsArray();
		if (color.AsArray().size() == 3) {
			SVG_Proto::RGB rgb;
			rgb.set_red(color.AsArray()[0].AsInt());
			rgb.set_green(color.AsArray()[1].AsInt());
			rgb.set_blue(color.AsArray()[2].AsInt());
			settings.mutable_underlayer_color()->mutable_color_rgb()->CopyFrom(rgb);
		}
		else if (color.AsArray().size() == 4) {
			SVG_Proto::RGBa rgba;
			rgba.set_red(color.AsArray()[0].AsInt());
			rgba.set_green(color.AsArray()[1].AsInt());
			rgba.set_blue(color.AsArray()[2].AsInt());
			rgba.set_opacity(color.AsArray()[3].AsDouble());
			settings.mutable_underlayer_color()->mutable_color_rgba()->CopyFrom(rgba);
		}
		else if (base.AsDict().at("underlayer_color").IsString()) {
			settings.mutable_underlayer_color()->set_color_str(color.AsString());
		}
	}

	for (Node colors : base.AsDict().at("color_palette").AsArray()) {
		if (colors.IsArray()) {
			if (colors.AsArray().size() == 3) {
				SVG_Proto::RGB rgb;
				rgb.set_red(colors.AsArray()[0].AsInt());
				rgb.set_green(colors.AsArray()[1].AsInt());
				rgb.set_blue(colors.AsArray()[2].AsInt());

				settings.add_color_palette()->mutable_color_rgb()->CopyFrom(rgb);
			}
			else if (colors.AsArray().size() == 4) {
				SVG_Proto::RGBa rgba;
				rgba.set_red(colors.AsArray()[0].AsInt());
				rgba.set_green(colors.AsArray()[1].AsInt());
				rgba.set_blue(colors.AsArray()[2].AsInt());
				rgba.set_opacity(colors.AsArray()[3].AsDouble());
				settings.add_color_palette()->mutable_color_rgba()->CopyFrom(rgba);
			}
		}
		else if (colors.IsString()) {
			settings.add_color_palette()->set_color_str(colors.AsString());
		}
	}
	return settings;
}

void SerializationBusesAndStopsInfo(transport_catalogue::TransportCatalogue& catalogue, TC_Proto::TransportCatalogue& tc) {
	int bus_index = 0;
	int stop_index = 0;
	for (auto& bus : catalogue.GetBusInfo()) {
		TC_Proto::BusInfo bus_info;
		bus.second.index = bus_index;
		bus_info.set_bus_name(bus.second.bus_name.data());
		bus_info.set_curvature(bus.second.curvature);
		bus_info.set_route_length_on_road(bus.second.route_length_on_road);

		auto bus_ptr = catalogue.FindBus(bus.second.bus_name);
		for (const auto& s : bus_ptr->stops_vector) {
			bus_info.add_stops_indexes(s->index);
		}

		bus_info.set_stops_on_route(bus.second.stops_on_route);
		bus_info.set_unique_stops(bus.second.unique_stops);
		bus_info.set_is_ring(catalogue.FindBus(bus.second.bus_name)->is_ring);
		tc.add_buses_info()->CopyFrom(bus_info);
		bus_index++;
	}
	for (const auto& stop : catalogue.GetStops()) {
		TC_Proto::StopInfo stop_info;
		stop_info.set_stop_name(stop.stop_name.data());
		stop_info.set_lat(stop.coodinates.lat);
		stop_info.set_lng(stop.coodinates.lng);
		stop_info.set_index(stop_index);

		Stop* stop_finded = catalogue.FindStop(stop.stop_name);
		auto stop_info_finded = catalogue.GetStopInfo().find(stop_finded);
		if (stop_info_finded != catalogue.GetStopInfo().end()) {
			for (auto s : catalogue.GetStopInfo().at(stop_finded)) {
				const auto& bus_info = catalogue.GetBusInfo().at(s->bus_name);
				stop_info.add_bus_names_indexes(bus_info.index);
			}
		}
		tc.add_stops_info()->CopyFrom(stop_info);
		stop_index++;
	}
}

void SerializationRouterData(TransportRoter& router, TC_Proto::TransportCatalogue& tc) {
	auto router_data = router.GetRouter();
	auto graphs = router_data.GetGraph();
	auto edges = graphs.GetEdges();
	for (int i = 0; i < edges.size(); ++i) {
		Graph_proto::Edge edge;
		edge.set_from(edges[i].from);
		edge.set_to(edges[i].to);
		edge.set_span_count(edges[i].span_count);
		edge.mutable_weight()->set_wait(edges[i].weight.wait);
		edge.mutable_weight()->set_movement(edges[i].weight.movement);
		for (int j = 0; j < tc.buses_info().size(); ++j) {
			if (router.GetBusNameById(i) == tc.buses_info().Get(j).bus_name()) {
				edge.set_bus_name_index(j);
				break;
			}
		}
		tc.mutable_router()->add_edges()->CopyFrom(edge);
	}
	const auto& internal_data = router_data.GetInternalData();
	for (int index_from = 0; index_from < internal_data.size(); ++index_from) {
		Router_proto::StopsTo s;
		for (int index_to = 0; index_to < internal_data[index_from].size(); ++index_to) {
			Router_proto::Route r;
			if (internal_data.at(index_from).at(index_to).has_value()) {
				r.mutable_internal_data()->mutable_weight()->set_movement(internal_data.at(index_from).at(index_to).value().weight.movement);
				r.mutable_internal_data()->mutable_weight()->set_wait(internal_data.at(index_from).at(index_to).value().weight.wait);
				if (internal_data.at(index_from).at(index_to).value().prev_edge.has_value()) {
					r.mutable_internal_data()->set_prev_edge(internal_data.at(index_from).at(index_to).value().prev_edge.value());
				}
				else {
					r.mutable_internal_data()->set_prev_edge_null(true);
				}
			}
			else {
				r.set_is_null(true);
			}
			s.add_stops_to()->CopyFrom(r);
		}
		tc.mutable_router()->add_routes_internal_data()->CopyFrom(s);
	}
}
