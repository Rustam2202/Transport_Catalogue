#pragma once

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include <cassert>
#include "json.h"
#include <fstream>
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
	renderer::MapRenderer map;
	RequestHandler rh(catalogue, map);

	rh.InsertStops(base.AsMap().at("base_requests").AsArray());
	rh.InsertStopsDistances(base.AsMap().at("base_requests").AsArray());
	rh.InsertBuses(base.AsMap().at("base_requests").AsArray());

	// Render settings 
	{
		Node render_settings = base.AsMap().at("render_settings");
		map.SetWidth(render_settings.AsMap().at("width").AsDouble());
		map.SetHeight(render_settings.AsMap().at("height").AsDouble());
		map.SetPadding(render_settings.AsMap().at("padding").AsDouble());
		map.SetStopRadius(render_settings.AsMap().at("stop_radius").AsDouble());
		map.SetLineWidth(render_settings.AsMap().at("line_width").AsDouble());
		map.SetUnderlayerWidth(render_settings.AsMap().at("underlayer_width").AsDouble());
		map.SetBusLabelFontSize(render_settings.AsMap().at("bus_label_font_size").AsInt());
		map.SetStopLabelFontSize(render_settings.AsMap().at("stop_label_font_size").AsInt());
		map.SetBusLabelOffset(render_settings.AsMap().at("bus_label_offset").AsArray()[0].AsDouble(), render_settings.AsMap().at("bus_label_offset").AsArray()[1].AsDouble());
		map.SetStopLabelOffset(render_settings.AsMap().at("stop_label_offset").AsArray()[0].AsDouble(), render_settings.AsMap().at("stop_label_offset").AsArray()[1].AsDouble());
		//map.SetUnderlayerColor(render_settings.AsMap().at("underlayer_color").AsString());

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
	std::stringstream out_str;

	// Draw map
	{
		LOG_DURATION("Draw map");
		rh.SetZoom();
		rh.AddBusesData();
		rh.DrawMap();

		//map.Rendering(output);
		//map.Rendering(out_str);
		//map.Rendering(/*output*/ std::cout);
		//rh.RenderMap().Render(output);
		//json::Document doc_map; // Load string from output
	}

	{
		LOG_DURATION("Compile status");
		Array result;
		rh.CompileStats(base.AsMap().at("stat_requests").AsArray(), result);
		Document doc(std::move(result));
		json::Print(doc, output);
		return;

		/*std::fstream answer("s10_final_opentest_1_answer.json"s);
		std::istream& istrm(answer);
		Node answ = Load(istrm).GetRoot();
		Document doc_answ(answ);

		auto a=doc.GetRoot().AsArray().back().AsMap().at("map").AsString();
		auto b = doc_answ.GetRoot().AsArray().back().AsMap().at("map").AsString();
		assert(doc.GetRoot().AsArray().back().AsMap().at("map").AsString() == doc_answ.GetRoot().AsArray().back().AsMap().at("map").AsString());*/
	}
}


