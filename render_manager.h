//
// Created by Artem Kryukov on 28.12.2021.
//

#pragma once

#include "database.h"
#include "route_manager.h"
#include "coordinate_table.h"
#include "svg.h"
#include "json.h"
#include <string>
#include <memory>
#include <list>
#include <utility>


class RenderSettings {
public:
    RenderSettings &SetMapWidth(double map_width) {
        map_width_ = map_width;
        return *this;
    }

    RenderSettings &SetMapHeight(double map_height) {
        map_height_ = map_height;
        return *this;
    }

    RenderSettings &SetMapPadding(double map_padding) {
        map_padding_ = map_padding;
        return *this;
    }

    RenderSettings &SetStopRadius(double stop_radius) {
        stop_radius_ = stop_radius;
        return *this;
    }

    RenderSettings &SetLineWidth(double line_width) {
        line_width_ = line_width;
        return *this;
    }

    RenderSettings &SetStopLabelFontSize(int stop_label_font_size) {
        stop_label_font_size_ = stop_label_font_size;
        return *this;
    }

    RenderSettings &SetStopLabelOffset(Svg::Point stop_label_offset) {
        stop_label_offset_ = stop_label_offset;
        return *this;
    }

    RenderSettings &SetUnderLayerColor(Svg::Color under_layer_color) {
        under_layer_color_ = std::move(under_layer_color);
        return *this;
    }

    RenderSettings &SetUnderLayerWidth(double under_layer_width) {
        under_layer_width_ = under_layer_width;
        return *this;
    }

    RenderSettings &SetBusLabelFontSize(int bus_label_font_size) {
        bus_label_font_size_ = bus_label_font_size;
        return *this;
    }

    RenderSettings &SetBusLabelOffset(Svg::Point bus_label_offset) {
        bus_label_offset_ = bus_label_offset;
        return *this;
    }

    RenderSettings &SetOuterMargin(double outer_margin) {
        outer_margin_ = outer_margin;
        return *this;
    }

    void AddColorToPalette(Svg::Color color) {
        color_palette_.push_back(std::move(color));
    }

    void AddRenderLayer(std::string layer) {
        layers_.push_back(std::move(layer));
    }

private:
    friend class RenderManager;

    double map_width_ = 0;
    double map_height_ = 0;
    double map_padding_ = 0;
    double stop_radius_ = 0;
    double line_width_ = 0;
    double under_layer_width_ = 0;
    double outer_margin_ = 0;
    int stop_label_font_size_ = 0;
    int bus_label_font_size_ = 0;
    Svg::Point bus_label_offset_;
    Svg::Point stop_label_offset_;
    Svg::Color under_layer_color_;
    std::list<Svg::Color> color_palette_;
    std::list<std::string> layers_;
};

class RenderManager {
public:
    explicit RenderManager(std::shared_ptr<Database> &database,
                           std::shared_ptr<GraphManager> graphManager,
                           std::shared_ptr<RouteManager> routeManager) :
            database_(database),
            graph_manager_(graphManager),
            route_manager_(routeManager),
            map_document_(std::make_unique<Svg::Document>()),
            settings_(std::make_unique<RenderSettings>()),
            coordinate_table_(std::make_shared<CoordinateTable>(database)) {};


    std::unique_ptr<RenderSettings> &Settings() {
        return settings_;
    }

    void BuildMap();

    [[nodiscard]] std::string Map() const;

    [[nodiscard]] std::string
    Route(const Graph::Router<double>::RouteInfo &route_info) const;


private:
    Svg::Point GetImagePos(StopId stop_id) const;

    void UpdateIntervals();

    void DrawBuses();

    void DrawBus(BusId bus_id, Svg::Color &color);

    void DrawBusNames();

    void DrawBusName(BusId bus_id, Svg::Color &color);

    void DrawStops();

    void DrawStop(StopId stop_id);

    void DrawStopNames();

    void DrawStopName(StopId stop_id);

    void DrawRouteRectangle() const;

    void DrawRouteBuses(const Graph::Router<double>::RouteInfo &route_info) const;
    void DrawRouteBusLabels(const Graph::Router<double>::RouteInfo &route_info) const;
    void DrawRouteStops(const Graph::Router<double>::RouteInfo &route_info) const;
    void DrawRouteStopLabels(const Graph::Router<double>::RouteInfo &route_info) const;

    void DrawRouteBus(Graph::EdgeId edge_id) const;
    void DrawRouteBusLabel(Graph::EdgeId edge_id) const;
    void DrawRouteStop(StopId stop_id) const;
    void DrawRouteStopLabel(StopId stop_id) const;


    std::shared_ptr<Database> database_;
    std::shared_ptr<GraphManager> graph_manager_;
    std::shared_ptr<RouteManager> route_manager_;

    std::unique_ptr<Svg::Document> map_document_ = nullptr;

    mutable std::unique_ptr<Svg::Document> route_document_ = nullptr;

    std::unique_ptr<RenderSettings> settings_;

    std::shared_ptr<CoordinateTable> coordinate_table_ = nullptr;

    std::unordered_map<BusId, Svg::Color> bus_colors_;

    double x_step = 0, y_step = 0;

    static const std::unordered_map<std::string, void (RenderManager::*)()> RENDER_LAYERS;
    static const std::unordered_map<std::string, void (RenderManager::* const)(const Graph::Router<double>::RouteInfo &) const> RENDER_ROUTE_LAYERS;
};
