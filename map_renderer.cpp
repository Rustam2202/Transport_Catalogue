#include "map_renderer.h"

#include <algorithm>

namespace renderer {

	void MapRenderer::SetRender(
		double width,
		double height,
		double padding,
		double radius,
		double line_width,
		double underlayer_width,
		int bus_label_font_size,
		int stop_label_font_size,
		double bus_offset_dx, double bus_offset_dy,
		double stop_offset_dx, double stop_offset_dy
	) {
		settings_.width = width;
		settings_.height = height;
		settings_.padding = padding;
		settings_.stop_radius = radius;
		settings_.line_width = line_width;
		settings_.underlayer_width = underlayer_width;
		settings_.bus_label_font_size = bus_label_font_size;
		settings_.stop_label_font_size = stop_label_font_size;
		settings_.bus_label_offset = { bus_offset_dx,bus_offset_dy };
		settings_.stop_label_offset = { stop_offset_dx,stop_offset_dy };
	}

	void MapRenderer::SetUnderlayerColor(int r, int g, int b) {
		svg::Rgb rgb(r, g, b);
		svg::Color color(rgb);
	//	settings_.color_palette.push_back(std::move(color));
		settings_.underlayer_color = std::move(color);
	}
	void MapRenderer::SetUnderlayerColor(int r, int g, int b, double o) {
		svg::Rgba rgba(r, g, b, o);
		svg::Color color(rgba);
		settings_.underlayer_color = std::move(color);
	}
	void MapRenderer::SetColorPalette(int r, int g, int b) {
		svg::Rgb rgb(r, g, b);
		svg::Color color(rgb);
		settings_.color_palette.push_back(std::move(color));
	}
	void MapRenderer::SetColorPalette(int r, int g, int b, double o) {
		svg::Rgba rgba(r, g, b, o);
		svg::Color color(rgba);
		settings_.color_palette.push_back(std::move(color));
	}

	void MapRenderer::MakeSphereProjector(std::vector<geo::Coordinates> coordinates) {
		SphereProjector sp(coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding);
		sphere_projector_ = std::move(sp);
	}

	void MapRenderer::AddBusWithStops(std::string bus_name, bool is_ring, std::string_view stop_name, const geo::Coordinates& coordinate) {
		buses_[{bus_name, is_ring}].push_back({ stop_name, sphere_projector_(coordinate) });
		points_.push_back({ stop_name, sphere_projector_(coordinate) });
	}

	void MapRenderer::Sorting() {
		std::sort(points_.begin(), points_.end(), [](PointOnMap lhs, PointOnMap rhs) {return lhs.stop_name < rhs.stop_name; }); // execution::par - no effect
		auto last = std::unique(points_.begin(), points_.end(), [](const PointOnMap& lhs, const PointOnMap& rhs) {return lhs.stop_name == rhs.stop_name; });
		points_.erase(last, points_.end());
	}

	void MapRenderer::RenderBusesLines() {
		size_t color_numb = 0;
		for (auto bus : buses_) {
			if (bus.second.empty()) {
				continue;
			}
			svg::Polyline polyline;
			for (size_t i = 0; i < bus.second.size(); ++i) {
				polyline.AddPoint(bus.second[i].coordinates);
			}
			if (bus.first.second == false) {
				for (int i = bus.second.size() - 2; i >= 0; --i) {
					polyline.AddPoint(bus.second[i].coordinates);
				}
			}

			polyline.SetStrokeColor(settings_.color_palette[color_numb]);
			polyline.SetFillColor(); // "none"
			polyline.SetStrokeWidth(settings_.line_width);
			polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
			polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			objects_.Add(std::move(polyline));

			if (color_numb < settings_.color_palette.size() - 1) {
				color_numb++;
			}
			else {
				color_numb = 0;
			}
		}
	}

	void MapRenderer::RenderBusesNames() {
		size_t color_numb = 0;
		for (auto bus : buses_) {
			if (bus.second.empty()) {
				continue;
			}
			svg::Text text_main;
			svg::Text text_layer;

			text_main.SetPosition(bus.second.front().coordinates);
			text_main.SetOffset(settings_.bus_label_offset);
			text_main.SetFontSize(settings_.bus_label_font_size);
			text_main.SetFontFamily("Verdana");
			text_main.SetFontWeight("bold");
			text_main.SetData(bus.first.first);
			text_main.SetFillColor(settings_.color_palette[color_numb]);

			text_layer.SetPosition(bus.second.front().coordinates);
			text_layer.SetOffset(settings_.bus_label_offset);
			text_layer.SetFontSize(settings_.bus_label_font_size);
			text_layer.SetFontFamily("Verdana");
			text_layer.SetFontWeight("bold");
			text_layer.SetData(bus.first.first);
			text_layer.SetFillColor(settings_.underlayer_color);
			text_layer.SetStrokeColor(settings_.underlayer_color);
			text_layer.SetStrokeWidth(settings_.underlayer_width);
			text_layer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
			text_layer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

			objects_.Add(std::move(text_layer));
			objects_.Add(std::move(text_main));

			// IF not ring AND two different stops
			if (bus.first.second == false && bus.second.front().stop_name != bus.second.back().stop_name) {
				svg::Text text_main_2;
				svg::Text text_layer_2;

				text_main_2.SetPosition(bus.second.back().coordinates);
				text_main_2.SetOffset(settings_.bus_label_offset);
				text_main_2.SetFontSize(settings_.bus_label_font_size);
				text_main_2.SetFontFamily("Verdana");
				text_main_2.SetFontWeight("bold");
				text_main_2.SetData(bus.first.first);
				text_main_2.SetFillColor(settings_.color_palette[color_numb]);

				text_layer_2.SetPosition(bus.second.back().coordinates);
				text_layer_2.SetOffset(settings_.bus_label_offset);
				text_layer_2.SetFontSize(settings_.bus_label_font_size);
				text_layer_2.SetFontFamily("Verdana");
				text_layer_2.SetFontWeight("bold");
				text_layer_2.SetData(bus.first.first);
				text_layer_2.SetFillColor(settings_.underlayer_color);
				text_layer_2.SetStrokeColor(settings_.underlayer_color);
				text_layer_2.SetStrokeWidth(settings_.underlayer_width);
				text_layer_2.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
				text_layer_2.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

				objects_.Add(std::move(text_layer_2));
				objects_.Add(std::move(text_main_2));
			}

			if (color_numb < settings_.color_palette.size() - 1) {
				color_numb++;
			}
			else {
				color_numb = 0;
			}
		}
	}

	void MapRenderer::RenderCircle() {
		for (const PointOnMap& point : points_) {
			svg::Circle circle(point.coordinates, settings_.stop_radius);
			circle.SetFillColor("white");
			objects_.Add(circle);
		}
	}

	void MapRenderer::RenderStopsNames() {
		for (const PointOnMap& point : points_) {
			svg::Text text_main;
			svg::Text text_layer;

			text_main.SetPosition(point.coordinates);
			text_main.SetOffset(settings_.stop_label_offset);
			text_main.SetFontSize(settings_.stop_label_font_size);
			text_main.SetFontFamily("Verdana");
			text_main.SetData(point.stop_name);
			text_main.SetFillColor("black");

			text_layer.SetPosition(point.coordinates);
			text_layer.SetOffset(settings_.stop_label_offset);
			text_layer.SetFontSize(settings_.stop_label_font_size);
			text_layer.SetFontFamily("Verdana");
			text_layer.SetFillColor(settings_.underlayer_color);
			text_layer.SetStrokeColor(settings_.underlayer_color);
			text_layer.SetStrokeWidth(settings_.underlayer_width);
			text_layer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
			text_layer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			text_layer.SetData(point.stop_name);

			objects_.Add(std::move(text_layer));
			objects_.Add(std::move(text_main));
		}
	}
}