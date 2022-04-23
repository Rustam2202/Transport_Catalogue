#include "json_reader.h"
#include "request_handler.h"

using namespace transport_catalogue;
using namespace json;
using namespace renderer;

void InsertStops(TransportCatalogue& catalogue, Array base) {
	for (Node base_data : base) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Stop stop;
			stop.stop_name = base_data.AsMap().at("name").AsString();
			stop.coodinates.lat = base_data.AsMap().at("latitude").AsDouble();
			stop.coodinates.lng = base_data.AsMap().at("longitude").AsDouble();
			catalogue.AddStop(std::move(stop));
		}
	}
}

void InsertStopsDistances(TransportCatalogue& catalogue, Array base) {
	for (Node base_data : base) {
		if (base_data.AsMap().at("type").AsString() == "Stop") {
			Dict stops = base_data.AsMap().at("road_distances").AsMap();
			for (auto other_stop : stops) {
				catalogue.SetDistanceBetweenStops(base_data.AsMap().at("name").AsString(), other_stop.first, other_stop.second.AsInt());
			}
		}
	}
}

void InsertBuses(TransportCatalogue& catalogue, Array base) {
	for (Node base_data : base) {
		if (base_data.AsMap().at("type").AsString() == "Bus") {
			Bus bus;
			bus.bus_name = base_data.AsMap().at("name").AsString();
			bus.is_ring = base_data.AsMap().at("is_roundtrip").AsBool();
			for (Node stop : base_data.AsMap().at("stops").AsArray()) {
				bus.stops_unique.insert(catalogue.FindStop(stop.AsString()));
				bus.stops_vector.push_back(catalogue.FindStop(stop.AsString()));
			}
			catalogue.AddBus(std::move(bus));
		}
	}
}

void SetMapRender(MapRenderer& map, Node render_settings) {
	map.SetRender(
		render_settings.AsMap().at("width").AsDouble(),
		render_settings.AsMap().at("height").AsDouble(),
		render_settings.AsMap().at("padding").AsDouble(),
		render_settings.AsMap().at("stop_radius").AsDouble(),
		render_settings.AsMap().at("line_width").AsDouble(),
		render_settings.AsMap().at("underlayer_width").AsDouble(),
		render_settings.AsMap().at("bus_label_font_size").AsInt(),
		render_settings.AsMap().at("stop_label_font_size").AsInt(),
		render_settings.AsMap().at("bus_label_offset").AsArray()[0].AsDouble(), render_settings.AsMap().at("bus_label_offset").AsArray()[1].AsDouble(),
		render_settings.AsMap().at("stop_label_offset").AsArray()[0].AsDouble(), render_settings.AsMap().at("stop_label_offset").AsArray()[1].AsDouble()
	);

	if (render_settings.AsMap().at("underlayer_color").IsString()) {
		map.SetUnderlayerColor(render_settings.AsMap().at("underlayer_color").AsString());
	}
	else if (render_settings.AsMap().at("underlayer_color").IsArray()) {
		Node color = render_settings.AsMap().at("underlayer_color").AsArray();
		if (color.AsArray().size() == 3) {
			map.SetUnderlayerColor(color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt());
		}
		else if (color.AsArray().size() == 4) {
			map.SetUnderlayerColor(color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt(), color.AsArray()[3].AsDouble());
		}
	}

	for (Node colors : render_settings.AsMap().at("color_palette").AsArray()) {
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

void CompileStats(RequestHandler& rh, Array base, Array& stats) {
	for (Node stat_data : base) {
		if (stat_data.AsMap().at("type").AsString() == "Stop") {
			stats.push_back(rh.MakeDictStop(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString()));
		}
		else if (stat_data.AsMap().at("type").AsString() == "Bus") {
			stats.push_back(rh.MakeDictBus(stat_data.AsMap().at("id").AsInt(), stat_data.AsMap().at("name").AsString()));
		}
		else if (stat_data.AsMap().at("type").AsString() == "Map") {
			stats.push_back(rh.MakeDictMap(stat_data.AsMap().at("id").AsInt()));
		}
	}
}

void PrintStats(RequestHandler& rh, Array base, std::ostream& output) {
	Array result;
	CompileStats(rh, base, result);
	Document doc(std::move(result));
	json::Print(doc, output);
}

void ReadJSON(std::istream& input, std::ostream& output) {
	Node base = Load(input).GetRoot();

	TransportCatalogue catalogue;
	MapRenderer map;
	RequestHandler rh(catalogue, map);

	InsertStops(catalogue, base.AsMap().at("base_requests").AsArray());
	InsertStopsDistances(catalogue, base.AsMap().at("base_requests").AsArray());
	InsertBuses(catalogue, base.AsMap().at("base_requests").AsArray());
	SetMapRender(map, base.AsMap().at("render_settings"));

	rh.SetZoom();
	rh.AddBusesData();
	rh.DrawMap();

	PrintStats(rh, base.AsMap().at("stat_requests").AsArray(), output);
}
