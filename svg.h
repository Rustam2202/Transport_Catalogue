#pragma once

#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace svg {

	//svg::Color color1;								 // none
	//svg::Color color2 = svg::Rgb{ 215, 30, 25 };       // rgb(215,30,25)
	//svg::Color color3 = svg::NoneColor;                // none
	//svg::Color color4 = svg::Rgba{ 15, 15, 25, 0.7 };  // rgba(15,15,25,0.7)
	//svg::Color color5 = "red"s;                        // red
	//svg::Color color6 = svg::Rgb{};                    // rgb(0,0,0)
	//svg::Color color7 = svg::Rgba{};                   // rgba(0,0,0,1.0);

	class Rgb {
	public:
		Rgb() {};
		Rgb(uint8_t r, uint8_t g, uint8_t b) {
			red = (uint8_t)r;
			green = (uint8_t)g;
			blue = (uint8_t)b;
		}

		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
	};

	class Rgba {
	public:
		Rgba() = default;
		Rgba(uint8_t r, uint8_t g, uint8_t b, double o) {
			red = r;
			green = g;
			blue = b;
			opacity = o;
		}

		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
		double opacity = 1.0;
	};

	using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
	inline const Color NoneColor = {};
	//using Color = std::string;
	// inline const Color NoneColor{ "none" };

	//class Color {
	//public:
	//	std::ostream& out;
	//	Color(): color_(std::monostate) {
	//	//	color_ = std::monostate;
	//	}
	//	void operator()(std::monostate) const {
	//		out << "none";
	//	}
	//	void operator()(std::string color_) const {
	//		out << color_;
	//	}
	//	void operator()(svg::Rgb color_) const {
	//		out << "rgb(" << color_.red << "," << color_.green << "," << color_.blue << ")";
	//	}
	//	void operator()(svg::Rgba color_) const {
	//		out << "rgba(" << color_.red << "," << color_.green << "," << color_.blue << "," << color_.opacity << ")";
	//	}
	//private:
	//	std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba> color_;
	//};

	struct Point {
		Point() = default;
		Point(double x, double y) : x(x), y(y) {}
		double x = 0;
		double y = 0;
	};

	struct RenderContext {
		RenderContext(std::ostream& out) : out(out) {}

		RenderContext(std::ostream& out, int indent_step, int indent = 0) : out(out), indent_step(indent_step), indent(indent) {}

		RenderContext Indented() const {
			return { out, indent_step, indent + indent_step };
		}

		void RenderIndent() const {
			for (int i = 0; i < indent; ++i) {
				out.put(' ');
			}
		}

		std::ostream& out;
		int indent_step = 0;
		int indent = 0;
	};

	enum class StrokeLineCap {
		BUTT,
		ROUND,
		SQUARE,
	};

	enum class StrokeLineJoin {
		ARCS,
		BEVEL,
		MITER,
		MITER_CLIP,
		ROUND,
	};

	struct OstreamColorPrint {
		std::ostream& out;
		void operator()(std::monostate) const {
			out << "none";
		}
		void operator()(std::string str) const {
			out << str;
		}
		void operator()(svg::Rgb rgb_color) const {
			out << "rgb(" << (int)rgb_color.red << "," << (int)rgb_color.green << "," << (int)rgb_color.blue << ")";
		}
		void operator()(svg::Rgba rgba_color) const {
			out << "rgba(" << (int)rgba_color.red << "," << (int)rgba_color.green << "," << (int)rgba_color.blue << "," << (double)rgba_color.opacity << ")";
		}
	};

	std::ostream& operator<<(std::ostream& os, const StrokeLineCap& stroke_linecap);
	std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& stroke_linejoin);

	template <typename Owner>
	class PathProps {
	public:
		Owner& SetFillColor(Color color = std::monostate{} /*NoneColor*/) {
			fill_color_ = std::move(color);
			return AsOwner();
		}
		Owner& SetStrokeColor(Color color = std::monostate{} /*NoneColor*/) {
			stroke_color_ = std::move(color);
			return AsOwner();
		}
		Owner& SetStrokeWidth(double width) {
			stroke_width_ = width;
			return AsOwner();
		}
		Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
			stroke_linecap_ = line_cap;
			return AsOwner();
		}
		Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
			stroke_linejoin_ = line_join;
			return AsOwner();
		}

	protected:
		~PathProps() = default;
	public:
		void RenderAttrs(std::ostream& out) const {
			using namespace std::literals;

			if (fill_color_) {
				std::ostringstream strm;
				std::visit(OstreamColorPrint{ strm }, *fill_color_);
				out << " fill=\""sv << strm.str() << "\""sv;
				//out << " fill=\""sv << *fill_color_ << "\""sv;
			}
			if (stroke_color_) {
				std::ostringstream strm;
				std::visit(OstreamColorPrint{ strm }, *stroke_color_);
				out << " stroke=\""sv << strm.str() << "\""sv;
				//out << " stroke=\""sv << *stroke_color_ << "\""sv;
			}
			if (stroke_width_) {
				out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
			}
			if (stroke_linecap_) {
				out << " stroke-linecap =\"" << *stroke_linecap_ << "\""sv;
			}
			if (stroke_linejoin_) {
				out << " stroke-linejoin=\"" << *stroke_linejoin_ << "\""sv;
			}
		}

	private:
		Owner& AsOwner() {
			return static_cast<Owner&>(*this);
		}

		std::optional<Color> fill_color_;
		std::optional<Color> stroke_color_;
		std::optional<double> stroke_width_;
		std::optional<StrokeLineCap> stroke_linecap_;
		std::optional<StrokeLineJoin> stroke_linejoin_;

		void operator()(std::monostate) const {
			std::ostream& out;
			out << "none";
		}
		void operator()(std::string str) const {
			std::ostream& out;
			out << str;
		}
		void operator()(svg::Rgb rgb_color) const {
			std::ostream& out;
			out << "rgb(" << rgb_color.red << "," << rgb_color.green << "," << rgb_color.blue << ")";
		}
		void operator()(svg::Rgba rgba_color) const {
			std::ostream& out;
			out << "rgba(" << rgba_color.red << "," << rgba_color.green << "," << rgba_color.blue << "," << rgba_color.opacity << ")";
		}
	};

	class Object {
	public:
		void Render(const RenderContext& context) const;

		virtual ~Object() = default;

	private:
		virtual void RenderObject(const RenderContext& context) const = 0;
	};

	class Circle : public Object, public PathProps<Circle> {
	public:
		Circle() = default;
		Circle(Point center, double radius) {
			center_ = center;
			radius_ = radius;
		}
		Circle& SetCenter(Point center);
		Circle& SetRadius(double radius);

	private:
		void RenderObject(const RenderContext& context) const override;
		Point center_;
		double radius_ = 1.0;
	};

	class Polyline : public Object, public PathProps<Polyline> {
	public:
		Polyline& AddPoint(Point point); 		// Добавляет очередную вершину к ломаной линии

	private:
		void RenderObject(const RenderContext& context) const override;
		Point center_;
		std::vector<Point> points_;
	};

	class Text : public Object, public PathProps<Text> {

	public:
		Text& SetPosition(Point pos); 					// Задаёт координаты опорной точки (атрибуты x и y)
		Text& SetOffset(Point offset);					// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
		Text& SetFontSize(uint32_t size);				// Задаёт размеры шрифта (атрибут font-size)
		Text& SetFontFamily(std::string font_family);	// Задаёт название шрифта (атрибут font-family)
		Text& SetFontWeight(std::string font_weight);	// Задаёт толщину шрифта (атрибут font-weight)
		Text& SetData(std::string_view data);			// Задаёт текстовое содержимое объекта (отображается внутри тега text)

	private:
		void RenderObject(const RenderContext& context) const override;
		Point center_;
		Point offset_;
		uint32_t font_size_ = 1;
		std::string font_family_;
		std::string font_weight_;
		std::string data_;
	};

	class ObjectContainer {
	public:

		virtual	void AddPtr(std::unique_ptr<Object>&& obj) = 0;

		template <typename Obj>
		void Add(Obj obj) {
			AddPtr(std::make_unique<Obj>(std::move(obj)));
		}

		virtual ~ObjectContainer() {}
	};

	class Drawable {
	public:
		virtual void Draw(ObjectContainer& container) const = 0;
		virtual ~Drawable() {}
	};

	class Document :public ObjectContainer {
	public:

		// Добавляет в svg-документ объект-наследник svg::Object
		void AddPtr(std::unique_ptr<Object>&& obj) {
			objects_.push_back(std::move(obj));
		}

		void Render(std::ostream& out) const;		// Выводит в ostream svg-представление документа

	private:
		std::vector<std::unique_ptr<Object>> objects_;
	};


}  // namespace svg

