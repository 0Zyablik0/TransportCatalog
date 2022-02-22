//
// Created by Artem Kryukov on 29.12.2021.
//

#include "svg.h"

template<typename Type>
void Svg::RenderProperty(const std::string &property, const Type &value, std::ostream &stream) {
    stream << ' ' << property;
    stream << "=\"";
    value.Render(stream);
    stream << "\" ";
}

void Svg::RenderProperty(const std::string &property, const double value, std::ostream &stream) {
    stream << ' ' << property;
    stream << "=\"" << value << "\" ";
}


void Svg::RenderProperty(const std::string &property, const std::string &value, std::ostream &stream) {
    stream << ' ' << property;
    stream << "=\"" << value << "\" ";
}

void Svg::RenderProperty(const std::string &property, int value, std::ostream &stream) {
    stream << ' ' << property;
    stream << "=\"" << value << "\" ";
}

void Svg::Rgb::Render(std::ostream &stream) const {
    stream << "rgb(" << red << ',' << green << ',' << blue << ")";
}

void Svg::RgbA::Render(std::ostream &stream) const {
    stream << "rgba(" << red << ',' << green << ',' << blue << ',' << alpha << ")";
}

void Svg::Color::Render(std::ostream &stream) const {
    if (holds_alternative<std::monostate>(color_)) {
        stream << "none";
    } else if (holds_alternative<std::string>(color_)) {
        stream << std::get<std::string>(color_);
    } else if (holds_alternative<Rgb>(color_))
        std::get<Rgb>(color_).Render(stream);
    else {
        std::get<RgbA>(color_).Render(stream);
    }
}

void Svg::Circle::Render(std::ostream &stream) {
    stream << "<" << "circle";
    RenderProperty("cx", center_.x, stream);
    RenderProperty("cy", center_.y, stream);
    RenderProperty("r", radius_, stream);
    Figure<Circle>::Render(stream);
    stream << "/>";

}

void Svg::Rectangle::Render(std::ostream & stream) {
    stream << "<" << "rect";
    RenderProperty("x", top_left_corner_.x, stream);
    RenderProperty("y", top_left_corner_.y, stream);
    RenderProperty("width", width_, stream);
    RenderProperty("height", height_, stream);
    Figure<Rectangle>::Render(stream);
    stream << "/>";
}

void Svg::Text::Render(std::ostream &stream) {
    stream << "<text";
    Figure<Text>::Render(stream);
    RenderProperty("x", point_.x, stream);
    RenderProperty("y", point_.y, stream);
    RenderProperty("dx", offset_.x, stream);
    RenderProperty("dy", offset_.y, stream);
    RenderProperty("font-size", (int) font_size_, stream);
    if (!font_family_.empty())
        RenderProperty("font-family", font_family_, stream);
    if (!font_weight_.empty())
        RenderProperty("font-weight", font_weight_, stream);
    stream << ">";
    stream << data_;
    stream << "</text>";
}

void Svg::Polyline::Render(std::ostream &stream) {
    stream << "<polyline";
    std::stringstream points_stream;
    points_stream.precision(stream.precision());
    for (auto &point: points_) {
        points_stream << point.x << ',' << point.y << ' ';
    }
    RenderProperty("points", points_stream.str(), stream);
    Figure<Polyline>::Render(stream);
    stream << "/>";
}

void Svg::Document::Render(std::ostream &stream) {
    stream.precision(10);
    stream << R"(<?xml version="1.0" encoding="UTF-8" ?>)";
    stream << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)";
    for (const auto &object: objects_) {
        object->Render(stream);
    }
    stream << R"(</svg>)";
}