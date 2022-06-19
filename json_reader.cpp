#include "json_reader.h"
#include "request_handler.h"
#include "transport_router.h"

using namespace transport_catalogue;
using namespace json;
using namespace renderer;

void InsertStops(TransportCatalogue& catalogue, Array base) {
	for (Node base_data : base) {
		if (base_data.AsDict().at("type").AsString() == "Stop") {
			Stop stop;
			stop.stop_name = base_data.AsDict().at("name").AsString();
			stop.coodinates.lat = base_data.AsDict().at("latitude").AsDouble();
			stop.coodinates.lng = base_data.AsDict().at("longitude").AsDouble();
			catalogue.AddStop(std::move(stop));
		}
	}
}

void InsertStopsDistances(TransportCatalogue& catalogue, Array base) {
	for (Node base_data : base) {
		if (base_data.AsDict().at("type").AsString() == "Stop") {
			Dict stops = base_data.AsDict().at("road_distances").AsDict();
			for (auto other_stop : stops) {
				catalogue.SetDistanceBetweenStops(base_data.AsDict().at("name").AsString(), other_stop.first, other_stop.second.AsInt());
			}
		}
	}
}

void InsertBuses(TransportCatalogue& catalogue, Array base) {
	for (Node base_data : base) {
		if (base_data.AsDict().at("type").AsString() == "Bus") {
			Bus bus;
			bus.bus_name = base_data.AsDict().at("name").AsString();
			bus.is_ring = base_data.AsDict().at("is_roundtrip").AsBool();
			for (Node stop : base_data.AsDict().at("stops").AsArray()) {
				bus.stops_unique.insert(catalogue.FindStop(stop.AsString()));
				bus.stops_vector.push_back(catalogue.FindStop(stop.AsString()));
			}
			catalogue.AddBus(std::move(bus));
		}
	}
}

void SetMapRender(MapRenderer& map, Node render_settings) {
	map.SetRender(
		render_settings.AsDict().at("width").AsDouble(),
		render_settings.AsDict().at("height").AsDouble(),
		render_settings.AsDict().at("padding").AsDouble(),
		render_settings.AsDict().at("stop_radius").AsDouble(),
		render_settings.AsDict().at("line_width").AsDouble(),
		render_settings.AsDict().at("underlayer_width").AsDouble(),
		render_settings.AsDict().at("bus_label_font_size").AsInt(),
		render_settings.AsDict().at("stop_label_font_size").AsInt(),
		render_settings.AsDict().at("bus_label_offset").AsArray()[0].AsDouble(), render_settings.AsDict().at("bus_label_offset").AsArray()[1].AsDouble(),
		render_settings.AsDict().at("stop_label_offset").AsArray()[0].AsDouble(), render_settings.AsDict().at("stop_label_offset").AsArray()[1].AsDouble()
	);

	if (render_settings.AsDict().at("underlayer_color").IsString()) {
		map.SetUnderlayerColor(render_settings.AsDict().at("underlayer_color").AsString());
	}
	else if (render_settings.AsDict().at("underlayer_color").IsArray()) {
		Node color = render_settings.AsDict().at("underlayer_color").AsArray();
		if (color.AsArray().size() == 3) {
			map.SetUnderlayerColor(color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt());
		}
		else if (color.AsArray().size() == 4) {
			map.SetUnderlayerColor(color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt(), color.AsArray()[3].AsDouble());
		}
	}

	for (Node colors : render_settings.AsDict().at("color_palette").AsArray()) {
		if (colors.IsArray()) {
			if (colors.AsArray().size() == 3) {
				map.SetColorPalette(colors.AsArray()[0].AsInt(), colors.AsArray()[1].AsInt(), colors.AsArray()[2].AsInt());
			}
			else if (colors.AsArray().size() == 4) {
				map.SetColorPalette(colors.AsArray()[0].AsInt(), colors.AsArray()[1].AsInt(), colors.AsArray()[2].AsInt(), colors.AsArray()[3].AsDouble());
			}
		}
		else if (colors.IsString()) {
			map.SetColorPalette(colors.AsString());
		}
	}
}

void SetRouter(TransportRouter& router, Node routing_settings) {
	router.SetBusWaitTime(routing_settings.AsDict().at("bus_wait_time").AsInt());
	router.SetBusVelocity(routing_settings.AsDict().at("bus_velocity").AsInt());
}

Node CompileStats(RequestHandler& rh, Array base) {
	Builder result;
	result.StartArray();
	for (Node stat_data : base) {
		if (stat_data.AsDict().at("type").AsString() == "Stop") {
			result.Value(rh.MakeDictStop(stat_data.AsDict().at("id").AsInt(), stat_data.AsDict().at("name").AsString()).GetValue());
		}
		else if (stat_data.AsDict().at("type").AsString() == "Bus") {
			result.Value(rh.MakeDictBus(stat_data.AsDict().at("id").AsInt(), stat_data.AsDict().at("name").AsString()).GetValue());
		}
		else if (stat_data.AsDict().at("type").AsString() == "Map") {
			result.Value(rh.MakeDictMap(stat_data.AsDict().at("id").AsInt()).GetValue());
		}
	}
	return result.EndArray().Build();
}

void PrintStats(RequestHandler& rh, Array base, std::ostream& output) {
	Document doc(CompileStats(rh, base));
	json::Print(doc, output);
}

void ReadJSON(std::istream& input, std::ostream& output) {
	Node base = Load(input).GetRoot();

	TransportCatalogue catalogue;
	MapRenderer map;
	RequestHandler handler(catalogue, map);

	InsertStops(catalogue, base.AsDict().at("base_requests").AsArray());
	InsertStopsDistances(catalogue, base.AsDict().at("base_requests").AsArray());
	InsertBuses(catalogue, base.AsDict().at("base_requests").AsArray());
	SetMapRender(map, base.AsDict().at("render_settings"));

	TransportRouter router(catalogue);
	SetRouter(router, base.AsDict().at("routing_settings"));

	handler.SetZoom();
	handler.AddBusesData();
	handler.DrawMap();

	PrintStats(handler, base.AsDict().at("stat_requests").AsArray(), output);
}
