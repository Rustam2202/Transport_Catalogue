#pragma once

#include "domain.h"
#include "geo.h"
#include "svg.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

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
		SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding);

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
	};

	class MapRenderer {
	public:

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
		void SetUnderlayerColor(int r, int g, int b);
		void SetUnderlayerColor(int r, int g, int b, double o);
		void SetColorPalette(std::string color) { settings_.color_palette.push_back(color); }
		void SetColorPalette(int r, int g, int b);
		void SetColorPalette(int r, int g, int b, double o);

		void MakeSphereProjector(std::vector<geo::Coordinates> coordinates);
		void AddBusWithStops(std::string bus_name, bool is_ring, std::string_view stop_name, const geo::Coordinates& coordinate);
		void Sorting();

		void AddBusesLines();
		void AddBusesNames();
		void AddCircle();
		void AddStopsNames();

		void Rendering(std::ostream& output) { objects_.Render(output); }

	private:
		svg::Document objects_;
		RenderSettings settings_;
		SphereProjector sphere_projector_;
		std::map<std::pair<std::string, bool>, std::vector<PointOnMap>> buses_;
		std::vector<PointOnMap> points_;
	};

	template <typename PointInputIt>
	SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding) : padding_(padding) {
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

} // namespace renderer

