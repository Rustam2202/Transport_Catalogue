#include "serialization.h"
#include "router.h"

TC_Proto::RenderSettings MakeRendersettings(Node base);

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
	tc.mutable_render_settings()->CopyFrom(MakeRendersettings(base.AsDict().at("render_settings")));

	auto router_data = router.GetRouter();
	auto graphs = router_data.GetGraph();
	auto edges = graphs.GetEdges();
	for (int i = 0; i < edges.size(); ++i) {
		TC_Proto::Edge edge;
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

	auto internal_data = router_data.GetInternalData();
	for (const auto& stops_to : internal_data) {
		TC_Proto::StopsTo stops_to_serializ;
		for (const auto& stop_from : stops_to) {
			TC_Proto::RouteInternalData intern_data_ser;
			if (stop_from.value().prev_edge) {
				intern_data_ser.set_prev_edge(stop_from.value().prev_edge.value());
			}
			else {
				intern_data_ser.set_prev_edge_null(true);
			}
			intern_data_ser.mutable_weight()->set_movement(stop_from.value().weight.movement);
			intern_data_ser.mutable_weight()->set_wait(stop_from.value().weight.wait);
			stops_to_serializ.add_stops_to()->CopyFrom(intern_data_ser);
		}
		tc.mutable_router()->add_routes_internal_data()->CopyFrom(stops_to_serializ);
	}

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

	Builder result;
	result.StartArray();
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
				result.StartDict()
					.Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt())
					.Key("error_message"s).Value("not found"s)
					.EndDict();
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
				result.StartDict()
					.Key("buses"s).Value(buses_arr)
					.Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt())
					.Key("request_id").Value(stat_data.AsDict().at("id").AsInt())
					.EndDict();

			}
		}
		else if (stat_data.AsDict().at("type").AsString() == "Bus") {
			size_t i = 0;
			bool founded = false;

			for (auto it = tc.buses_info().begin(); it != tc.buses_info().end(); ++it, ++i) {
				if (stat_data.AsDict().at("name").AsString() == (*it).bus_name()) {
					founded = true;
					break;
				}
			}
			if (founded == false) {
				result.StartDict()
					.Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt())
					.Key("error_message").Value("not found"s)
					.EndDict();
			}
			else {
				result.StartDict()
					.Key("curvature").Value(tc.buses_info().Get(i).curvature())
					.Key("request_id").Value(stat_data.AsDict().at("id").AsInt())
					.Key("route_length").Value(static_cast<double>(tc.buses_info().Get(i).route_length_on_road()))
					.Key("stop_count").Value(static_cast<int>(tc.buses_info().Get(i).stops_on_route()))
					//.Key("stop_count").Value(static_cast<int>(tc.buses_info().Get(i).stops_indexes_size()))
					.Key("unique_stop_count").Value(static_cast<int>(tc.buses_info().Get(i).unique_stops()))
					.EndDict();
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
			else /*if (tc.render_settings().underlayer_color().has_color_rgba())*/ {
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
			result.StartDict()
				.Key("map"s).Value(s.str())
				.Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt())
				.EndDict();
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

			auto route_internal_data = tc.router().routes_internal_data().Get(index_from).stops_to().Get(index_to);
			double total_time = route_internal_data.weight().movement() + route_internal_data.weight().wait();

			std::vector<uint64_t> edges;
			for (std::optional<uint64_t> edge_id = route_internal_data.prev_edge();
				//	!tc.router().routes_internal_data().Get(index_from).stops_to().Get(tc.router().edges().Get(*edge_id).from()).prev_edge_null();
				edge_id;
				//edge_id = tc.router().routes_internal_data().Get(index_from).stops_to().Get(tc.router().edges().Get(*edge_id).from()).prev_edge()
				) {
				edges.push_back(*edge_id);
				if (!tc.router().routes_internal_data().Get(index_from).stops_to().Get(tc.router().edges().Get(*edge_id).from()).prev_edge_null()) {
					edge_id = tc.router().routes_internal_data().Get(index_from).stops_to().Get(tc.router().edges().Get(*edge_id).from()).prev_edge();
				}
				else {
					edge_id = nullopt;
				}
			}
			if (edges.empty()) {
				result.StartDict().Key("request_id"s).Value(stat_data.AsDict().at("id").AsInt()).Key("error_message"s).Value("not found"s).EndDict();
			}
			else {
				std::reverse(edges.begin(), edges.end());
				result.StartDict().Key("items").StartArray();
				for (auto edge : edges) {
					result.StartDict()
						.Key("stop_name").Value(tc.stops_info().Get(tc.router().edges().Get(edge).from()).stop_name())
						.Key("time").Value(tc.router().edges().Get(edge).weight().wait())
						.Key("type").Value("Wait"s)
						.EndDict();
					result.StartDict()
						.Key("bus").Value(tc.buses_info().Get(tc.router().edges().Get(edge).bus_name_index()).bus_name())
						.Key("span_count").Value(tc.router().edges().Get(edge).span_count())
						.Key("time").Value(tc.router().edges().Get(edge).weight().movement())
						.Key("type").Value("Bus"s)
						.EndDict();
				}
				result.EndArray();
				result.Key("request_id").Value(stat_data.AsDict().at("id").AsInt());
				result.Key("total_time").Value(total_time).EndDict();
			}
		}
	}

	result.EndArray();
	json::Print(Document(result.Build()), output);
}

TC_Proto::RenderSettings MakeRendersettings(Node base) {
	TC_Proto::RenderSettings settings;
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
			TC_Proto::RGB rgb;
			rgb.set_red(color.AsArray()[0].AsInt());
			rgb.set_green(color.AsArray()[1].AsInt());
			rgb.set_blue(color.AsArray()[2].AsInt());
			settings.mutable_underlayer_color()->mutable_color_rgb()->CopyFrom(rgb);
		}
		else if (color.AsArray().size() == 4) {
			TC_Proto::RGBa rgba;
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
				TC_Proto::RGB rgb;
				rgb.set_red(colors.AsArray()[0].AsInt());
				rgb.set_green(colors.AsArray()[1].AsInt());
				rgb.set_blue(colors.AsArray()[2].AsInt());

				settings.add_color_palette()->mutable_color_rgb()->CopyFrom(rgb);
			}
			else if (colors.AsArray().size() == 4) {
				TC_Proto::RGBa rgba;
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

