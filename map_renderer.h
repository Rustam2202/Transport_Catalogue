#pragma once

#include "domain.h"
#include "geo.h"
#include "svg.h"

#include <algorithm>
#include <execution>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

using namespace std::literals;

namespace renderer {

	inline const double EPSILON = 1e-6;
	inline bool IsZero(double value) {
		return std::abs(value) < EPSILON;
	}

	class SphereProjector {
	public:
		SphereProjector() {}

		template <typename PointInputIt>
		SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding) : padding_(padding) {
			if (points_begin == points_end) {
				return;
			}

			const auto [left_it, right_it]
				= std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
				return lhs.lng < rhs.lng;
					});
			min_lon_ = left_it->lng;
			const double max_lon = right_it->lng;

			const auto [bottom_it, top_it]
				= std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
				return lhs.lat < rhs.lat;
					});
			const double min_lat = bottom_it->lat;
			max_lat_ = top_it->lat;

			std::optional<double> width_zoom;
			if (!IsZero(max_lon - min_lon_)) {
				width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
			}

			std::optional<double> height_zoom;
			if (!IsZero(max_lat_ - min_lat)) {
				height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
			}

			if (width_zoom && height_zoom) {
				zoom_coeff_ = std::min(*width_zoom, *height_zoom);
			}
			else if (width_zoom) {
				zoom_coeff_ = *width_zoom;
			}
			else if (height_zoom) {
				zoom_coeff_ = *height_zoom;
			}
		}

		svg::Point operator()(geo::Coordinates coords) const {
			return { (coords.lng - min_lon_) * zoom_coeff_ + padding_,
					(max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
		}

	private:
		double padding_;
		double min_lon_ = 0;
		double max_lat_ = 0;
		double zoom_coeff_ = 0;
	};

	struct PointOnMap {
		std::string_view stop_name;
		svg::Point coordinates;
	};

	struct RenderSettings
	{
		double width = 0.0;
		double height = 0.0;
		double padding = 0.0;
		double stop_radius = 0.0;
		double line_width = 0.0;
		double underlayer_width = 0.0;
		int bus_label_font_size = 0;
		int stop_label_font_size = 0;
		svg::Point stop_label_offset{ 0,0 };
		svg::Point bus_label_offset{ 0,0 };
		svg::Color underlayer_color;
		std::vector<svg::Color> color_palette;
		//SphereProjector sphere_projector;
	};

	class MapRenderer {
	public:
		//MapRenderer() = default;
		//MapRenderer(std::vector<geo::Coordinates> coordinates):	sphere_projector_(coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding) {}

		void SetWidth(double width) { settings_.width = width; }
		void SetHeight(double height) { settings_.height = height; }
		void SetPadding(double padding) { settings_.padding = padding; }
		void SetStopRadius(double radius) { settings_.stop_radius = radius; }
		void SetLineWidth(double width) { settings_.line_width = width; }
		void SetUnderlayerWidth(double width) { settings_.underlayer_width = width; }
		void SetBusLabelFontSize(int size) { settings_.bus_label_font_size = size; }
		void SetStopLabelFontSize(int size) { settings_.stop_label_font_size = size; }
		void SetStopLabelOffset(double dx, double dy) { settings_.stop_label_offset = { dx,dy }; }
		void SetBusLabelOffset(double dx, double dy) { settings_.bus_label_offset = { dx,dy }; }
		void SetUnderlayerColor(std::string color) { settings_.underlayer_color = color; }
		void SetUnderlayerColor(int r, int g, int b) {
			svg::Rgb rgb(r, g, b);
			svg::Color color(rgb);
			settings_.color_palette.push_back(std::move(color));
		}
		void SetUnderlayerColor(int r, int g, int b, double o) {
			svg::Rgba rgba(r, g, b, o);
			svg::Color color(rgba);
			settings_.underlayer_color = std::move(color);
		}
		void SetColorPalette(std::string color) { settings_.color_palette.push_back(color); }
		void SetColorPalette(int r, int g, int b) {
			svg::Rgb rgb(r, g, b);
			svg::Color color(rgb);
			settings_.color_palette.push_back(std::move(color));
			//	settings_.color_palette.push_back({ r,g,b });
		}
		void SetColorPalette(int r, int g, int b, double o) {
			svg::Rgba rgba(r, g, b, o);
			svg::Color color(rgba);
			settings_.color_palette.push_back(std::move(color));
		}

		void MakeSphereProjector(std::vector<geo::Coordinates> coordinates) {
			SphereProjector sp(coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding);
			sphere_projector_ = std::move(sp);
		}

		void AddBusWithStops(std::string bus_name, std::string_view stop_name, const geo::Coordinates& coordinate) {
			buses_[bus_name].push_back({ stop_name, sphere_projector_(coordinate) });
			points_.push_back({ stop_name, sphere_projector_(coordinate) });
		}

		void Sorting() {
			std::sort(/*std::execution::par,*/ points_.begin(), points_.end(), [](PointOnMap lhs, PointOnMap rhs) {return lhs.stop_name < rhs.stop_name; });
			auto last = std::unique(/*std::execution::par,*/ points_.begin(), points_.end(), [](const PointOnMap& lhs, const PointOnMap& rhs) {return lhs.stop_name == rhs.stop_name; });
			points_.erase(last, points_.end());
		}

		void AddBusesLines() {
			size_t color_numb = 0;
			for (auto bus : buses_) {
				if (bus.second.empty()) {
					continue;
				}
				svg::Polyline polyline;
				for (int i = 0; i < bus.second.size(); ++i) {
					polyline.AddPoint(bus.second[i].coordinates);
				}
				if (bus.second.front().stop_name != bus.second.back().stop_name) {
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

		void AddBusesNames() {
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
				text_main.SetData(bus.first);
				text_main.SetFillColor(settings_.color_palette[color_numb]);

				text_layer.SetPosition(bus.second.front().coordinates);
				text_layer.SetOffset(settings_.bus_label_offset);
				text_layer.SetFontSize(settings_.bus_label_font_size);
				text_layer.SetFontFamily("Verdana");
				text_layer.SetFontWeight("bold");
				text_layer.SetData(bus.first);
				text_layer.SetFillColor(settings_.underlayer_color);
				text_layer.SetStrokeColor(settings_.underlayer_color);
				text_layer.SetStrokeWidth(settings_.underlayer_width);
				text_layer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
				text_layer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

				objects_.Add(std::move(text_layer));
				objects_.Add(std::move(text_main));

				// if not ring
				if (bus.second.front().stop_name != bus.second.back().stop_name) {
					svg::Text text_main_2;
					svg::Text text_layer_2;

					text_main_2.SetPosition(bus.second.back().coordinates);
					text_main_2.SetOffset(settings_.bus_label_offset);
					text_main_2.SetFontSize(settings_.bus_label_font_size);
					text_main_2.SetFontFamily("Verdana");
					text_main_2.SetFontWeight("bold");
					text_main_2.SetData(bus.first);
					text_main_2.SetFillColor(settings_.color_palette[color_numb]);

					text_layer_2.SetPosition(bus.second.back().coordinates);
					text_layer_2.SetOffset(settings_.bus_label_offset);
					text_layer_2.SetFontSize(settings_.bus_label_font_size);
					text_layer_2.SetFontFamily("Verdana");
					text_layer_2.SetFontWeight("bold");
					text_layer_2.SetData(bus.first);
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

		void AddCircle() {
			for (auto point : points_) {
				svg::Circle circle(point.coordinates, settings_.stop_radius);
				circle.SetFillColor("white");
				objects_.Add(circle);
			}
		}

		void AddStopsNames() {
			for (auto point : points_) {
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

		void Rendering(std::ostream& output) {
			objects_.Render(output);
		}

	private:
		svg::Document objects_;
		RenderSettings settings_;
		SphereProjector sphere_projector_;
		std::map<std::string, std::vector<PointOnMap>> buses_;
		std::vector<PointOnMap> points_;
	};

} // namespace renderer