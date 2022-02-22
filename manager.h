//
// Created by Artem Kryukov on 24.12.2021.
//

#pragma once

#include <vector>
#include "route_manager.h"
#include "render_manager.h"
#include "database.h"
#include "requests.h"
#include <memory>
#include <iostream>
#include "json.h"



using Dict = std::map<std::string, Json::Node>;
using Array = std::vector<Json::Node>;
using StopsMap = std::vector<std::shared_ptr<Stop::Stop>>;
using BusesMap = std::vector<std::shared_ptr<Bus>>;

class Manager {
public:

    Manager() {
        database_ = std::make_shared<Database>();
        graph_manager_ = std::make_shared<GraphManager>(database_);
        route_manager_ = std::make_shared<RouteManager>();
        render_manager_ = std::make_shared<RenderManager>(database_, graph_manager_, route_manager_);
    }

    Json::Document JSONRequest(const Json::Document &requests, std::ostream &output = std::cout);

private:

    void BaseRequests(const Json::Node &requests);

    [[nodiscard]] Array StatsRequests(const Json::Node &requests) const;

    void RoutingSettingsRequests(const Json::Node &requests);

    void RenderSettingsRequests(const Json::Node &request);

    void InputStopRequest(const Json::Node &request);

    void InputBusRequest(const Json::Node &request);

    [[nodiscard]] Dict BusRequest(const Json::Node &request) const;

    [[nodiscard]] Dict StopRequest(const Json::Node &request) const;

    [[nodiscard]] Dict RouteRequest(const Json::Node &request) const;

    [[nodiscard]] Dict MapRequest(const Json::Node &request) const;

    [[nodiscard]] Array BuildRoute(Graph::Router<double>::RouteInfo route_info) const;


    std::shared_ptr<Database> database_;

    std::shared_ptr<GraphManager> graph_manager_;
    std::shared_ptr<RouteManager> route_manager_;
    std::shared_ptr<RenderManager> render_manager_;
};
