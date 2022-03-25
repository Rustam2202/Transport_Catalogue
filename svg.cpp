#include "svg.h"

namespace svg {

	using namespace std::literals;

	std::ostream& operator<<(std::ostream& os, const StrokeLineCap& stroke_linecap) {
		switch (stroke_linecap)
		{
		case StrokeLineCap::BUTT:
			os << "butt";
			break;
		case StrokeLineCap::ROUND:
			os << "round";
			break;
		case StrokeLineCap::SQUARE:
			os << "square";
			break;
		}
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& stroke_linejoin) {
		switch (stroke_linejoin)
		{
		case StrokeLineJoin::ARCS:
			os << "arcs";
			break;
		case StrokeLineJoin::BEVEL:
			os << "bevel";
			break;
		case StrokeLineJoin::MITER:
			os << "miter";
			break;
		case StrokeLineJoin::MITER_CLIP:
			os << "miter-clip";
			break;
		case StrokeLineJoin::ROUND:
			os << "round";
			break;
		}
		return os;
	}

	void Object::Render(const RenderContext& context) const {
		context.RenderIndent();

		// Делегируем вывод тега своим подклассам
		RenderObject(context);

		context.out << std::endl;
	}

	Circle& Circle::SetCenter(Point center) {
		center_ = center;
		return *this;
	}

	Circle& Circle::SetRadius(double radius) {
		radius_ = radius;
		return *this;
	}

	void Circle::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
		out << "r=\""sv << radius_ << "\" "sv;
		RenderAttrs(context.out);
		out << "/>"sv;
	}

	Polyline& Polyline::AddPoint(Point point) {
		points_.push_back(point);
		return *this;
	}

	void Polyline::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<polyline points=\""sv;
		if (points_.size()) {
			size_t i;
			for (i = 0; i < points_.size() - 1; ++i) {
				out << points_[i].x << "," << points_[i].y << " ";
			}
			out << points_[i].x << "," << points_[i].y;
		}
		out << "\" "sv;
		RenderAttrs(context.out);

		out << "/>"sv;
	}

	Text& Text::SetPosition(Point pos) {
		center_ = pos;
		return *this;
	}

	Text& Text::SetOffset(Point offset) {
		offset_.x = offset.x;
		offset_.y = offset.y;
		return *this;
	}

	Text& Text::SetFontSize(uint32_t size) {
		font_size_ = size;
		return *this;
	}

	Text& Text::SetFontFamily(std::string font_family) {
		font_family_ = font_family;
		return *this;
	}

	Text& Text::SetFontWeight(std::string font_weight) {
		font_weight_ = font_weight;
		return *this;
	}

	Text& Text::SetData(std::string_view data) {
		data_ = data;
		return *this;
	}

	void Text::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<text x=\""sv << center_.x << "\" y=\""sv << center_.y << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y;
		out << "\" font-size=\""sv << font_size_ << "\""sv;
		if (!font_family_.empty()) {
			out << " font-family=\""sv << font_family_ << "\""sv;
		}
		if (!font_weight_.empty()) {
			out << " font-weight=\""sv << font_weight_ << "\""sv;
		}
		RenderAttrs(context.out);

		out << ">"sv;

		for (auto ch : data_) {
			if (ch == '\"') {
				out << "&quot;";
			}
			else if (ch == '\'') {
				out << "&apos;";
			}
			else if (ch == '<') {
				out << "&lt;";
			}
			else if (ch == '>') {
				out << "&gt;";
			}
			else if (ch == '&') {
				out << "&amp;";
			}
			else {
				out << ch;
			}
		}
		out << "</text>"sv;
	}

	void Document::Render(std::ostream& out) const {
		using namespace std::literals;
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
		out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
		RenderContext ctx(out, 2, 2);
		for (const auto& object : objects_) {
			object.get()->Render(ctx);
		}
		out << "</svg>\n"sv;
	}

}  // namespace svg