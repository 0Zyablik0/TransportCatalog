//
// Created by Artem Kryukov on 28.12.2021.
//

#pragma once

#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <iostream>
#include <sstream>


namespace Svg {
    enum Channels {
        Red = 0,
        Green = 1,
        Blue = 2,
        Alpha = 3,
    };

    struct Point {
        double x = 0, y = 0;
    };

    struct Rgb {
        short red = 0, green = 0, blue = 0;

        void Render(std::ostream &stream) const;
    };

    struct RgbA {
        short red = 0, green = 0, blue = 0;
        double alpha = 0.0;

        void Render(std::ostream &stream) const;
    };


    class Color {
    public:
        Color() : color_(std::monostate()) {};

        Color(std::string color) : color_(std::move(color)) {};

        Color(const char *color) : color_(std::string(color)) {};

        Color(Rgb color) : color_(color) {};

        Color(RgbA color) : color_(color) {};

        void Render(std::ostream &stream) const;

    private:
        std::variant<std::monostate, std::string, Rgb, RgbA> color_;
    };

    const Color NoneColor{};

    template<typename Type>
    void RenderProperty(const std::string &property, const Type &value, std::ostream &stream);

    void RenderProperty(const std::string &property, const double value, std::ostream &stream);

    void RenderProperty(const std::string &property, const std::string &value, std::ostream &stream);

    void RenderProperty(const std::string &property, int value, std::ostream &stream);

    class Object {
    public:
        virtual void Render(std::ostream &) = 0;

        virtual ~Object() = default;

    };


    template<class Type>
    class Figure {
    public:

        Figure() = default;


        Type &SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsType();
        };

        Type &SetStrokeColor(const Color &color) {
            stroke_color_ = color;
            return AsType();
        }

        Type &SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsType();
        }

        Type &SetStrokeLineCap(const std::string &line_cap) {
            stroke_line_cap_ = line_cap;
            return AsType();
        }

        Type &SetStrokeLineJoin(const std::string &line_join) {
            stroke_line_join_ = line_join;
            return AsType();
        }

        void Render(std::ostream &stream) {
            RenderProperty("fill", fill_color_, stream);
            RenderProperty("stroke", stroke_color_, stream);
            RenderProperty("stroke-width", stroke_width_, stream);
            if (!stroke_line_cap_.empty()) {
                RenderProperty("stroke-linecap", stroke_line_cap_, stream);
            }
            if (!stroke_line_join_.empty()) {
                RenderProperty("stroke-linejoin", stroke_line_join_, stream);
            }
        }


    private:
        Type &AsType() {
            return static_cast<Type &>(*this);
        }

        Color fill_color_ = NoneColor;
        Color stroke_color_ = NoneColor;
        double stroke_width_ = 1.0;
        std::string stroke_line_cap_;
        std::string stroke_line_join_;
    };


    class Polyline : public Object, public Figure<Polyline> {
    public:

        Polyline &AddPoint(Point point) {
            points_.push_back(point);
            return *this;
        }

        void Render(std::ostream &stream) override;

        ~Polyline() override = default;

    private:
        std::vector<Point> points_;

    };

    class Circle : public Object, public Figure<Circle> {
    public:
        Circle() = default;

        Circle &SetCenter(Point point) {
            center_ = point;
            return *this;
        }

        Circle &SetRadius(double radius) {
            radius_ = radius;
            return *this;
        }

        void Render(std::ostream &stream) override;

        ~Circle() override = default;

    private:
        Point center_;
        double radius_ = 1.0;
    };

    class Rectangle : public Object, public Figure<Rectangle> {
    public:
        Rectangle() = default;

        Rectangle& SetTopCorner(Point point){
            top_left_corner_ = point;
            return *this;
        }

        Rectangle& SetHeight(double height){
            height_ = height;
            return *this;
        }

        Rectangle& SetWidth(double width){
            width_ = width;
            return *this;
        }

        void Render(std::ostream &) override;

    private:
        Point top_left_corner_;
        double width_ = 0, height_ = 0;
    };

    class Text : public Object, public Figure<Text> {
    public:
        Text() = default;

        Text &SetPoint(Point point) {
            point_ = point;
            return *this;
        }

        Text &SetOffset(Point offset) {
            offset_ = offset;
            return *this;
        }

        Text &SetFontSize(uint32_t size) {
            font_size_ = size;
            return *this;
        }

        Text &SetFontFamily(const std::string &font_family) {
            font_family_ = font_family;
            return *this;
        }

        Text &SetData(const std::string &data) {
            data_ = data;
            return *this;
        }

        Text &SetFontWeight(const std::string &font_weight) {
            font_weight_ = font_weight;
            return *this;
        }

        void Render(std::ostream &stream) override;

    private:
        Point point_, offset_;
        uint32_t font_size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;

    };

    class Document {
    public:
        Document() = default;


        template<class ObjectType>
        void Add(ObjectType object) {
            objects_.emplace_back(std::make_unique<ObjectType>(object));
        }

        void Render(std::ostream &stream);

    private:
        std::vector<std::shared_ptr<Object>> objects_;
    };

}