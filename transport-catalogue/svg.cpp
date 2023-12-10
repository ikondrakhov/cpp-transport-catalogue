#include "svg.h"
#include <algorithm>

namespace svg {

using namespace std::literals;
    
std::ostream& operator<<(std::ostream& out, Color c) {
    visit(ColorPrinter{out}, c);
    return out;
}
    
std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap) {
    switch(line_cap) {
        case StrokeLineCap::BUTT:
            out << "butt";
            break;
        case StrokeLineCap::ROUND:
            out << "round";
            break;
        case StrokeLineCap::SQUARE:
            out << "square";
            break;
    }
    return out;
}
    
std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join) {
    switch(line_join) {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case StrokeLineJoin::MITER:
            out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            out << "round";
            break;
    }
    return out;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(out);
    out << "/>"sv;
}
    
    
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}
    
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    if(points_.size() != 0) {
        std::for_each(points_.begin(), points_.end() - 1, [&out](const Point& p) {
            out << p.x << ","sv << p.y << " ";
        });
        out << points_[points_.size() - 1].x << ","sv << points_[points_.size() - 1].y;
    }
    out << "\""sv;
    RenderAttrs(out);
    out << " />"sv;
}
    
// Задаёт координаты опорной точки (атрибуты x и y)
Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

std::string Text::ReplaceAll(std::string text, char c, const std::string& str) {
    size_t last_find = 0;
    while(text.find(c, last_find) != std::string::npos) {
        last_find = text.find(c, last_find);
        text.replace(last_find, 1, str);
        last_find += str.size();
    }
    return text;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    data = ReplaceAll(data, '&', "&amp;"s);
    data = ReplaceAll(data, '\"', "&quot;"s);
    data = ReplaceAll(data, '\'', "&apos;"s);
    data = ReplaceAll(data, '<', "&lt;"s);
    data = ReplaceAll(data, '>', "&gt;"s);
    text_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text"sv;
    out << " x=\"" << position_.x << "\" y=\"" << position_.y << "\"";
    out << " dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\"";
    out << " font-size=\"" << font_size_ << "\"";
    if(font_family_.size()) {
        out << " font-family=\"" << font_family_ << "\"";
    }
    if(font_weight_.size()) {
        out << " font-weight=\"" << font_weight_ << "\"";
    }
    RenderAttrs(out);
    out << ">" << text_ <<"</text>";
}
    
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.push_back(std::move(obj));
}
    
void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    for(const auto& obj: objects_) {
        obj->Render(out);
    }
    out << "</svg>";
}

}  // namespace svg