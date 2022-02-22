//
// Created by Artem Kryukov on 24.12.2021.
//

#include "manager.h"
#include <unordered_map>
#include <string>
#include <algorithm>
#include <memory>

Svg::Color AsColor(const Json::Node &node);

Json::Document Manager::JSONRequest(const Json::Document &requests, std::ostream &output) {
    auto dicts = requests.GetRoot().AsMap();

    if (dicts.count("base_requests")) {
        BaseRequests(dicts["base_requests"]);
    }
    if (dicts.count("routing_settings")) {
        RoutingSettingsRequests(dicts["routing_settings"]);
    }

    if (dicts.count("render_settings")) {
        RenderSettingsRequests(dicts["render_settings"]);
        render_manager_->BuildMap();
    }
    if (dicts.count("stat_requests")) {
        return Json::Document(StatsRequests(dicts["stat_requests"]));
    }

    return Json::Document({Dict()});
}

void Manager::BaseRequests(const Json::Node &requests) {
    std::unordered_map<std::string, Request::Type> types = {
            {"Stop", Request::Type::STOP_IN},
            {"Bus",  Request::Type::BUS_IN},
    };
    for (const auto &request: requests.AsArray()) {

        switch (types[request.AsMap().at("type").AsString()]) {
            case Request::Type::STOP_IN: {
                InputStopRequest(request);
                break;
            }
            case Request::Type::BUS_IN: {
                InputBusRequest(request);
                break;
            }
            default:
                throw std::runtime_error("Wrong type of base request");
        }
    }
}

void Manager::RoutingSettingsRequests(const Json::Node &requests) {
    graph_manager_
            ->SetBusVelocity(requests.AsMap().at("bus_velocity").AsDouble())
            .SetWaitTime(requests.AsMap().at("bus_wait_time").AsInt());
    graph_manager_->BuildGraph();
    *route_manager_ = graph_manager_->BuildRouteManager();
}

void Manager::RenderSettingsRequests(const Json::Node &request) {
    render_manager_->Settings()->SetMapWidth(request.AsMap().at("width").AsDouble());
    render_manager_->Settings()->SetMapHeight(request.AsMap().at("height").AsDouble());
    render_manager_->Settings()->SetMapPadding(request.AsMap().at("padding").AsDouble());
    render_manager_->Settings()->SetStopRadius(request.AsMap().at("stop_radius").AsDouble());
    render_manager_->Settings()->SetLineWidth(request.AsMap().at("line_width").AsDouble());
    render_manager_->Settings()->SetStopLabelFontSize(request.AsMap().at("stop_label_font_size").AsInt());
    render_manager_->Settings()->SetUnderLayerWidth(request.AsMap().at("underlayer_width").AsDouble());
    render_manager_->Settings()->SetUnderLayerColor(AsColor(request.AsMap().at("underlayer_color")));
    render_manager_->Settings()->SetBusLabelFontSize(request.AsMap().at("bus_label_font_size").AsInt());
    render_manager_->Settings()->SetOuterMargin(request.AsMap().at("outer_margin").AsDouble());
    render_manager_->Settings()->SetStopLabelOffset(
            {
                    .x = request.AsMap().at("stop_label_offset").AsArray()[0].AsDouble(),
                    .y = request.AsMap().at("stop_label_offset").AsArray()[1].AsDouble()
            }
    );
    render_manager_->Settings()->SetBusLabelOffset(
            {
                    .x = request.AsMap().at("bus_label_offset").AsArray()[0].AsDouble(),
                    .y = request.AsMap().at("bus_label_offset").AsArray()[1].AsDouble()
            }
    );
    for (const auto &layer: request.AsMap().at("layers").AsArray()) {
        render_manager_->Settings()->AddRenderLayer(layer.AsString());
    }
    for (const auto &color: request.AsMap().at("color_palette").AsArray()) {
        render_manager_->Settings()->AddColorToPalette(AsColor(color));
    }
}

Array Manager::StatsRequests(const Json::Node &requests) const {
    Array answers;
    std::unordered_map<std::string, Request::Type> types = {
            {"Bus",   Request::Type::BUS_OUT},
            {"Stop",  Request::Type::STOP_OUT},
            {"Route", Request::Type::ROUTE},
            {"Map",   Request::Type::MAP}
    };
    for (const auto &request: requests.AsArray()) {
        switch (types[request.AsMap().at("type").AsString()]) {
            case Request::Type::STOP_OUT: {
                answers.push_back(StopRequest(request));
                break;
            }
            case Request::Type::BUS_OUT: {
                answers.push_back(BusRequest(request));
                break;
            }
            case Request::Type::ROUTE: {
                answers.push_back(RouteRequest(request));
                break;
            }
            case Request::Type::MAP: {
                answers.push_back(MapRequest(request));
                break;
            }
            default:
                throw std::runtime_error("Wrong type of stats request");
        }
    }
    return answers;
}

void Manager::InputStopRequest(const Json::Node &request) {
    std::string stop = request.AsMap().at("name").AsString();
    StopId stop_id = database_->GetStopId(stop);
    database_->GetStop(stop_id)->SetLongitude(request.AsMap().at("longitude").AsDouble());
    database_->GetStop(stop_id)->SetLatitude(request.AsMap().at("latitude").AsDouble());
    if (request.AsMap().count("road_distances")) {
        for (const auto&[neighbour, distance]: request.AsMap().at("road_distances").AsMap()) {
            StopId neighbour_id = database_->GetStopId(neighbour);
            database_->GetStop(stop_id)->SetDistance(neighbour_id, distance.AsInt());
            if (!database_->GetStop(neighbour_id)->IsReachable(stop_id))
                database_->GetStop(neighbour_id)->SetDistance(stop_id, distance.AsInt());
        }
    }
}

void Manager::InputBusRequest(const Json::Node &request) {
    auto name = request.AsMap().at("name").AsString();
    std::vector<std::string> route;
    for (const auto &stop: request.AsMap().at("stops").AsArray())
        route.push_back(stop.AsString());
    database_->AddBus(name, route, request.AsMap().at("is_roundtrip").AsBool());
}

Dict Manager::BusRequest(const Json::Node &request) const {
    Dict answer;
    std::string bus_name = request.AsMap().at("name").AsString();
    answer["request_id"] = request.AsMap().at("id");
    if (!database_->IsBusInBase(bus_name)) {
        answer["error_message"] = Json::Node(std::string("not found"));
        return answer;
    }
    BusId bus_id = database_->GeBusId(bus_name);
    auto distances = database_->Buses()[bus_id]->Length();
    answer["route_length"] = Json::Node((int) distances.distance_);
    answer["curvature"] = Json::Node(double(distances.distance_) / distances.geometrical_distance);
    answer["stop_count"] = Json::Node(int(database_->GetBus(bus_id)->TotalStops()));
    answer["unique_stop_count"] = Json::Node(int(database_->GetBus(bus_id)->UniqueStops()));
    return answer;
}

Dict Manager::StopRequest(const Json::Node &request) const {
    Dict answer;
    answer["request_id"] = request.AsMap().at("id");
    std::string stop_name = request.AsMap().at("name").AsString();
    if (!database_->IsStopInBase(stop_name)) {
        answer["error_message"] = Json::Node(std::string("not found"));
        return answer;
    }
    StopId stop_id = database_->GetStopId(stop_name);
    Array buses;
    for (const auto &bus: database_->GetStop(stop_id)->GetBuses()) {
        auto elem = Json::Node(database_->GetBus(bus)->GetName());
        auto it = std::lower_bound(buses.begin(),
                                   buses.end(),
                                   elem,
                                   [](const Json::Node &lhs, const Json::Node &rhs) {
                                       return lhs.AsString() < rhs.AsString();
                                   });
        buses.insert(it, elem);
    }
    answer["buses"] = std::move(buses);
    return answer;
}

Dict Manager::RouteRequest(const Json::Node &request) const {
    Dict answer;
    answer["request_id"] = request.AsMap().at("id");
    auto from = request.AsMap().at("from").AsString();
    auto to = request.AsMap().at("to").AsString();
    auto from_vertex = graph_manager_->GetArrivalVertex(database_->GetStopId(from));
    auto to_vertex = graph_manager_->GetArrivalVertex(database_->GetStopId(to));
    auto optimal_route = route_manager_->BuildRoute(from_vertex, to_vertex);
    if (!optimal_route.has_value()) {
        answer["error_message"] = Json::Node(std::string("not found"));
        return answer;
    }
    answer["total_time"] = optimal_route->weight;
    answer["items"] = BuildRoute(optimal_route.value());
    answer["map"] = render_manager_->Route(optimal_route.value());
    return answer;
}

Array Manager::BuildRoute(Graph::Router<double>::RouteInfo route_info) const {
    Array items;
    int i = 0;
    while (i < route_info.edge_count) {
        Dict item;
        auto edge_id = route_manager_->GetRouteEdge(route_info.id, i);
        auto edge = graph_manager_->Edge(edge_id);
        if (graph_manager_->Bus(edge_id) == -1) {
            item["time"] = edge.weight;
            StopId stop_id = (StopId) edge.from / 2;
            item["stop_name"] = database_->GetStop(stop_id)->GetName();
            item["type"] = "Wait";
            items.push_back(item);
            i++;
        } else {
            item["type"] = "Bus";
            BusId bus_id = graph_manager_->Bus(edge_id);
            item["bus"] = database_->GetBus(bus_id)->GetName();
            item["time"] = edge.weight;
            item["span_count"] = Json::Node((int) graph_manager_->Count(edge_id));
            items.push_back(item);
            i++;
        }
    }
    return items;
}

Dict Manager::MapRequest(const Json::Node &request) const {
    Dict answer;
    answer["request_id"] = request.AsMap().at("id");
    answer["map"] = render_manager_->Map();
    return answer;
}

Svg::Color AsColor(const Json::Node &node) {
    if (std::holds_alternative<std::string>(node)) {
        return node.AsString();
    } else {
        auto color = node.AsArray();
        if (color.size() == 3) {
            return Svg::Rgb{
                    .red = (short) color[Svg::Channels::Red].AsInt(),
                    .green = (short) color[Svg::Channels::Green].AsInt(),
                    .blue = (short) color[Svg::Channels::Blue].AsInt()
            };
        } else if (color.size() == 4) {
            return Svg::RgbA{
                    .red = (short) color[Svg::Channels::Red].AsInt(),
                    .green = (short) color[Svg::Channels::Green].AsInt(),
                    .blue = (short) color[Svg::Channels::Blue].AsInt(),
                    .alpha = color[Svg::Channels::Alpha].AsDouble()
            };
        } else
            throw std::runtime_error("Wrong color format");
    }
}
