//
// Created by Artem Kryukov on 28.12.2021.
//

#include "render_manager.h"


std::string ToJsonFormat(const std::string &str) {
    std::stringstream stream(str);
    std::string json_string;
    char symbol = 0;
    while (stream.get(symbol)) {
        if (symbol == '"' or symbol == '\\') {
            json_string.push_back('\\');
        }
        json_string.push_back(symbol);
    }
    return json_string;
}


std::string RenderManager::Map() const {
    std::stringstream stream;
    stream.precision(10);
    map_document_->Render(stream);
    return ToJsonFormat(stream.str());
}

std::string RenderManager::Route(const Graph::Router<double>::RouteInfo& route_info) const {
    std::stringstream stream;
    stream.precision(10);
    route_document_ = std::make_unique<Svg::Document>(*map_document_);

    DrawRouteRectangle();
    for (auto &layer: settings_->layers_) {
        (this->*RENDER_ROUTE_LAYERS.at(layer))(route_info);
    }

    route_document_->Render(stream);
    return ToJsonFormat(stream.str());
}


void RenderManager::UpdateIntervals() {
    coordinate_table_->UpdateTable();
    if (coordinate_table_->MaxLongitude() > 1) {
        x_step = (settings_->map_width_ - 2 * settings_->map_padding_)
                 / (double) (coordinate_table_->MaxLongitude() - 1);
    } else x_step = 0;
    if (coordinate_table_->MaxLatitude() > 1) {
        y_step = (settings_->map_height_ - 2 * settings_->map_padding_)
                 / (double) (coordinate_table_->MaxLatitude() - 1);
    } else
        y_step = 0;
}

void RenderManager::BuildMap() {
    map_document_ = std::make_unique<Svg::Document>();
    UpdateIntervals();
    for (auto &layer: settings_->layers_) {
        (this->*RENDER_LAYERS.at(layer))();
    }
}

void RenderManager::DrawBuses() {
    auto color = settings_->color_palette_.begin();
    for (const auto&[bus_name, bus_id]: database_->GetBusList()) {
        DrawBus(bus_id, *color);
        bus_colors_[bus_id] = *color;
        color++;
        if (color == settings_->color_palette_.end()) {
            color = settings_->color_palette_.begin();
        }
    }
}

void RenderManager::DrawBus(BusId bus_id, Svg::Color &color) {
    auto bus = Svg::Polyline()
            .SetStrokeColor(color)
            .SetStrokeWidth(settings_->line_width_)
            .SetStrokeLineCap("round")
            .SetStrokeLineJoin("round");
    for (auto stop_id: database_->GetBus(bus_id)->Stops()) {
        bus.AddPoint(GetImagePos(stop_id));
    }
    if (!database_->GetBus(bus_id)->IsClosed()) {
        size_t stops_size = database_->GetBus(bus_id)->Stops().size();
        for (auto i = 1; i < stops_size; i++) {
            bus.AddPoint(GetImagePos(database_->GetBus(bus_id)->GetStop(stops_size - i - 1)));
        }
    }
    map_document_->Add(bus);
}

void RenderManager::DrawBusNames() {
    auto color = settings_->color_palette_.begin();
    for (const auto&[bus_name, bus_id]: database_->GetBusList()) {
        DrawBusName(bus_id, *color);
        color++;
        if (color == settings_->color_palette_.end()) {
            color = settings_->color_palette_.begin();
        }
    }
}

void RenderManager::DrawBusName(BusId bus_id, Svg::Color &color) {
    auto stops = database_->GetBus(bus_id)->Stops();
    auto layer = Svg::Text()
            .SetPoint(GetImagePos(stops.front()))
            .SetOffset(settings_->bus_label_offset_)
            .SetFontSize(settings_->bus_label_font_size_)
            .SetFontFamily("Verdana")
            .SetFontWeight("bold")
            .SetData(database_->GetBus(bus_id)->GetName())
            .SetFillColor(settings_->under_layer_color_)
            .SetStrokeColor(settings_->under_layer_color_)
            .SetStrokeWidth(settings_->under_layer_width_)
            .SetStrokeLineCap("round")
            .SetStrokeLineJoin("round");
    auto main_text = Svg::Text()
            .SetPoint(GetImagePos(stops.front()))
            .SetOffset(settings_->bus_label_offset_)
            .SetFontSize(settings_->bus_label_font_size_)
            .SetFontFamily("Verdana")
            .SetFontWeight("bold")
            .SetData(database_->GetBus(bus_id)->GetName())
            .SetFillColor(color);
    map_document_->Add(layer);
    map_document_->Add(main_text);
    if (!database_->GetBus(bus_id)->IsClosed() and stops.front() != stops.back()) {
        layer.SetPoint(GetImagePos(database_->GetBus(bus_id)->Stops().back()));
        main_text.SetPoint(GetImagePos(database_->GetBus(bus_id)->Stops().back()));
        map_document_->Add(layer);
        map_document_->Add(main_text);
    }
}

void RenderManager::DrawStops() {
    for (const auto&[stop_name, stop_id]: database_->GetStopList()) {
        DrawStop(stop_id);
    }
}

void RenderManager::DrawStop(StopId stop_id) {
    map_document_->Add(
            Svg::Circle()
                    .SetCenter(GetImagePos(stop_id))
                    .SetRadius(settings_->stop_radius_)
                    .SetFillColor("white")
    );
}

void RenderManager::DrawStopNames() {
    for (const auto&[stop_name, stop_id]: database_->GetStopList()) {
        DrawStopName(stop_id);
    }
}

void RenderManager::DrawStopName(StopId stop_id) {
    map_document_->Add(Svg::Text()
                               .SetPoint(GetImagePos(stop_id))
                               .SetOffset(settings_->stop_label_offset_)
                               .SetFontSize(settings_->stop_label_font_size_)
                               .SetFontFamily("Verdana")
                               .SetData(database_->GetStop(stop_id)->GetName())
                               .SetFillColor(settings_->under_layer_color_)
                               .SetStrokeColor(settings_->under_layer_color_)
                               .SetStrokeWidth(settings_->under_layer_width_)
                               .SetStrokeLineCap("round")
                               .SetStrokeLineJoin("round")
    );
    map_document_->Add(Svg::Text()
                               .SetPoint(GetImagePos(stop_id))
                               .SetOffset(settings_->stop_label_offset_)
                               .SetFontSize(settings_->stop_label_font_size_)
                               .SetFontFamily("Verdana")
                               .SetData(database_->GetStop(stop_id)->GetName())
                               .SetFillColor("black")
    );
}

void RenderManager::DrawRouteRectangle() const{
    route_document_->Add(Svg::Rectangle().SetHeight(settings_->map_height_ + 2 * settings_->outer_margin_)
                                 .SetWidth(settings_->map_width_ + 2 * settings_->outer_margin_)
                                 .SetFillColor(settings_->under_layer_color_)
                                 .SetTopCorner({.x =  -settings_->outer_margin_,
                                                       .y =  -settings_->outer_margin_})
    );
}

void RenderManager::DrawRouteBuses(const Graph::Router<double>::RouteInfo& route_info) const {
    for (int i = 0; i < route_info.edge_count; i ++){
        auto edge_id = route_manager_->GetRouteEdge(route_info.id, i);
        DrawRouteBus(edge_id);
    }
}

void RenderManager::DrawRouteBus(Graph::EdgeId edge_id) const {
    if (graph_manager_->Bus(edge_id) == -1) return;
    auto bus_id = graph_manager_->Bus(edge_id);
    auto bus = Svg::Polyline()
            .SetStrokeColor(bus_colors_.at(bus_id))
            .SetStrokeWidth(settings_->line_width_)
            .SetStrokeLineCap("round")
            .SetStrokeLineJoin("round");
    auto edge = graph_manager_->Edge(edge_id);
    StopId from_stop_id = (StopId) edge.from / 2;
    StopId to_stop_id = (StopId) edge.to / 2;
    int stop_pos = graph_manager_->GetStopPos(edge_id);
    auto bus_stops = database_->GetBusStops(bus_id);
    while (true){
        auto stop_id = bus_stops.at(abs(stop_pos));
        bus.AddPoint(GetImagePos(stop_id));
        if (stop_id == to_stop_id) break;
        stop_pos ++;
    }
    route_document_->Add(bus);
}

void RenderManager::DrawRouteBusLabels(const Graph::Router<double>::RouteInfo &route_info) const {
    for (int i = 0; i < route_info.edge_count; i ++){
        auto edge_id = route_manager_->GetRouteEdge(route_info.id, i);
        DrawRouteBusLabel(edge_id);
    }
}

void RenderManager::DrawRouteBusLabel(Graph::EdgeId edge_id) const {
    auto bus_id = graph_manager_->Bus(edge_id);
    if (bus_id == -1) return;
    auto edge = graph_manager_->Edge(edge_id);
    StopId from_stop_id = (StopId) edge.from / 2;
    StopId to_stop_id = (StopId) edge.to / 2;
    auto bus = database_->GetBus(bus_id);
    if (bus->IsEndStop(from_stop_id) ||  bus->IsEndStop(to_stop_id) ){
        auto layer = Svg::Text()
                .SetOffset(settings_->bus_label_offset_)
                .SetFontSize(settings_->bus_label_font_size_)
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(database_->GetBus(bus_id)->GetName())
                .SetFillColor(settings_->under_layer_color_)
                .SetStrokeColor(settings_->under_layer_color_)
                .SetStrokeWidth(settings_->under_layer_width_)
                .SetStrokeLineCap("round")
                .SetStrokeLineJoin("round");
        auto main_text = Svg::Text()
                .SetOffset(settings_->bus_label_offset_)
                .SetFontSize(settings_->bus_label_font_size_)
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(database_->GetBus(bus_id)->GetName())
                .SetFillColor(bus_colors_.at(bus_id));
        if (bus->IsEndStop(from_stop_id)){
            layer.SetPoint(GetImagePos(from_stop_id));
            main_text.SetPoint(GetImagePos(from_stop_id));
            route_document_->Add(layer);
            route_document_->Add(main_text);
        }
        if (bus->IsEndStop(to_stop_id)){
            layer.SetPoint(GetImagePos(to_stop_id));
            main_text.SetPoint(GetImagePos(to_stop_id));
            route_document_->Add(layer);
            route_document_->Add(main_text);
        }
    }
}

void RenderManager::DrawRouteStops(const Graph::Router<double>::RouteInfo &route_info) const {
    for (int i = 0; i < route_info.edge_count; i ++){
        auto edge_id = route_manager_->GetRouteEdge(route_info.id, i);
        auto bus_id = graph_manager_->Bus(edge_id);
        if (bus_id == -1) continue;
        auto edge = graph_manager_->Edge(edge_id);
        StopId from_stop_id = (StopId) edge.from / 2;
        StopId to_stop_id = (StopId) edge.to / 2;
        auto stops = database_->GetBus(bus_id)->Stops();
        int stop_pos = graph_manager_->GetStopPos(edge_id);
        while (true){
            auto stop_id = stops.at(abs(stop_pos));
            DrawRouteStop(stop_id);
            if (stop_id == to_stop_id) break;
            stop_pos ++;
        }
    }

}

void RenderManager::DrawRouteStop(StopId stop_id) const {
    route_document_->Add(
            Svg::Circle()
                    .SetCenter(GetImagePos(stop_id))
                    .SetRadius(settings_->stop_radius_)
                    .SetFillColor("white")
    );
}

void RenderManager::DrawRouteStopLabels(const Graph::Router<double>::RouteInfo &route_info) const {
    for (int i = 0; i < route_info.edge_count; i ++) {
        auto edge_id = route_manager_->GetRouteEdge(route_info.id, i);
        auto bus_id = graph_manager_->Bus(edge_id);
        if (bus_id == -1) continue;
        auto edge = graph_manager_->Edge(edge_id);
        StopId from_stop_id = (StopId) edge.from / 2;
        StopId to_stop_id = (StopId) edge.to / 2;
        DrawRouteStopLabel(from_stop_id);
    }
    if (route_info.edge_count == 0) return;
    auto edge_id = route_manager_->GetRouteEdge(route_info.id, route_info.edge_count - 1);
    auto bus_id = graph_manager_->Bus(edge_id);
    auto edge = graph_manager_->Edge(edge_id);
    DrawRouteStopLabel((StopId) edge.to/2);
}

void RenderManager::DrawRouteStopLabel(StopId stop_id) const {
    route_document_->Add(Svg::Text()
                               .SetPoint(GetImagePos(stop_id))
                               .SetOffset(settings_->stop_label_offset_)
                               .SetFontSize(settings_->stop_label_font_size_)
                               .SetFontFamily("Verdana")
                               .SetData(database_->GetStop(stop_id)->GetName())
                               .SetFillColor(settings_->under_layer_color_)
                               .SetStrokeColor(settings_->under_layer_color_)
                               .SetStrokeWidth(settings_->under_layer_width_)
                               .SetStrokeLineCap("round")
                               .SetStrokeLineJoin("round")
    );
    route_document_->Add(Svg::Text()
                               .SetPoint(GetImagePos(stop_id))
                               .SetOffset(settings_->stop_label_offset_)
                               .SetFontSize(settings_->stop_label_font_size_)
                               .SetFontFamily("Verdana")
                               .SetData(database_->GetStop(stop_id)->GetName())
                               .SetFillColor("black")
    );

}

Svg::Point RenderManager::GetImagePos(StopId stop_id) const{
    return {.x = x_step * coordinate_table_->GetLongitudePos(stop_id) + settings_->map_padding_,
            .y =  settings_->map_height_ - settings_->map_padding_
                  - y_step * coordinate_table_->GetLatitudePos(stop_id)};
}

const std::unordered_map<std::string, void (RenderManager::*)()>RenderManager::RENDER_LAYERS =
        {
                {"bus_lines",   &RenderManager::DrawBuses},
                {"bus_labels",  &RenderManager::DrawBusNames},
                {"stop_points", &RenderManager::DrawStops},
                {"stop_labels", &RenderManager::DrawStopNames}
        };


const std::unordered_map<std::string, void ( RenderManager:: *const)(const Graph::Router<double>::RouteInfo &) const>RenderManager::RENDER_ROUTE_LAYERS =
        {
                {"bus_lines",   &RenderManager::DrawRouteBuses},
                {"bus_labels",  &RenderManager::DrawRouteBusLabels},
                {"stop_points", &RenderManager::DrawRouteStops},
                {"stop_labels", &RenderManager::DrawRouteStopLabels}
        };
