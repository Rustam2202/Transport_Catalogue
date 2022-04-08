#pragma once

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "json.h"
#include "log_duration.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

using namespace transport_catalogue;
using namespace json;
using namespace std::literals;



inline void ReadJSON(std::istream& input = std::cin, std::ostream& output = std::cout) {
	Node base = Load(input).GetRoot();
	transport_catalogue::TransportCatalogue catalogue;
	renderer::MapRenderer mr;
	RequestHandler rh(catalogue, mr);

	rh.InsertStops(base.AsMap().at("base_requests").AsArray());
	rh.InsertStopsDistances(base.AsMap().at("base_requests").AsArray());
	rh.InsertBuses(base.AsMap().at("base_requests").AsArray());

	// Render settings 
	{
		Node render_settings = base.AsMap().at("render_settings");
		mr.SetWidth(render_settings.AsMap().at("width").AsDouble());
		mr.SetHeight(render_settings.AsMap().at("height").AsDouble());
		mr.SetPadding(render_settings.AsMap().at("padding").AsDouble());
		mr.SetStopRadius(render_settings.AsMap().at("stop_radius").AsDouble());
		mr.SetLineWidth(render_settings.AsMap().at("line_width").AsDouble());
		mr.SetUnderlayerWidth(render_settings.AsMap().at("underlayer_width").AsDouble());
		mr.SetBusLabelFontSize(render_settings.AsMap().at("bus_label_font_size").AsInt());
		mr.SetStopLabelFontSize(render_settings.AsMap().at("stop_label_font_size").AsInt());
		mr.SetBusLabelOffset({ render_settings.AsMap().at("bus_label_offset").AsArray()[0].AsDouble(), render_settings.AsMap().at("bus_label_offset").AsArray()[1].AsDouble() });
		mr.SetStopLabelOffset({ render_settings.AsMap().at("stop_label_offset").AsArray()[0].AsDouble(), render_settings.AsMap().at("stop_label_offset").AsArray()[1].AsDouble() });
		mr.SetUnderlayerColor(render_settings.AsMap().at("underlayer_color").AsString());
		for (Node colors : render_settings.AsMap().at("color_palette").AsArray()) {
			if (colors.IsArray()) {
				if (colors.AsArray().size() == 3) {
					mr.SetColorPalette(colors.AsArray()[0].AsInt(), colors.AsArray()[1].AsInt(), colors.AsArray()[2].AsInt());
				}
				else if (colors.AsArray().size() == 4) {
					mr.SetColorPalette(colors.AsArray()[0].AsInt(), colors.AsArray()[1].AsInt(), colors.AsArray()[2].AsInt(), colors.AsArray()[3].AsDouble());
				}
			}
			else if (colors.IsString()) {
				mr.SetColorPalette(colors.AsString());
			}
		}
	}

	Array result;
	rh.CompileStats(base.AsMap().at("stat_requests").AsArray(), result);
	Document doc(std::move(result));
	Print(doc, output);
}


