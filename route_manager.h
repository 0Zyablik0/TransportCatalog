//
// Created by Artem Kryukov on 28.12.2021.
//

#pragma once

#include <utility>

#include "database.h"
#include "graph.h"
#include "router.h"
#include "types.h"



class RouteManager {
public:
    RouteManager() = default;

    explicit RouteManager(const GraphPtr &graph) {
        router_ = std::make_unique<Graph::Router<double>>(*graph);
    };

    auto BuildRoute(Graph::VertexId from, Graph::VertexId to) const {
        return router_->BuildRoute(from, to);
    }

    Graph::EdgeId GetRouteEdge(Graph::Router<double>::RouteId route_id, size_t edge_idx) const {
        return router_->GetRouteEdge(route_id, edge_idx);
    }

private:
    std::unique_ptr<Graph::Router<double>> router_ = nullptr;
};


class GraphManager {
public:
    explicit GraphManager(std::shared_ptr<Database> database) : database_(std::move(database)) {};

    GraphManager &SetWaitTime(int time) {
        wait_time_ = time;
        return *this;
    }

    GraphManager &SetBusVelocity(double velocity) {
        if (velocity < 1) throw std::runtime_error("Bus velocity must be not less than 1");
        bus_velocity_ = velocity;
        return *this;
    }

    GraphManager &InitializeGraph(size_t num_vertices) {
        graph_ = std::make_shared<Graph::DirectedWeightedGraph<double>>(num_vertices);
        return *this;
    }


    void BuildGraph();


    auto BuildRouteManager() {
        if (!graph_) throw std::runtime_error("Graph wasn't initialized");
        return RouteManager{graph_};
    }


    auto Edge(Graph::EdgeId edge_id) const {
        return graph_->GetEdge(edge_id);
    }

    auto Bus(Graph::EdgeId edge_id) const {
        return edges_to_bus.at(edge_id);
    }

    auto Count(Graph::EdgeId edge_id) const {
        return edges_count.at(edge_id);
    }

    [[nodiscard]] Graph::VertexId GetArrivalVertex(StopId stop_id) const {
        return stop_id * 2 + 1;
    }

    [[nodiscard]] Graph::VertexId GetDepartureVertex(StopId stop_id) const {
        return stop_id * 2;
    }

    [[nodiscard]] int GetStopPos(Graph::EdgeId edge_id) const{
        return edges_to_stop_pos_.at(edge_id);
    }


private:
    void Validate() const {
        if (bus_velocity_ < 1) throw std::runtime_error("Bus velocity must be not less than 1");
        if (wait_time_ < 0) throw std::runtime_error("Waiting time must be non-negative");
        if (!database_) throw std::runtime_error("Database is not valid");
    }

    void AddStopEdge(StopId stop_id);

    void AddBusEdges(BusId bus_id);

    Graph::EdgeId AddEdge(StopId stop1, StopId stop2, BusId bus_id, double distance);



    double CalculateWeight(double distance) const;

    int wait_time_ = -1;
    double bus_velocity_ = 0;

    std::shared_ptr<Database> database_ = nullptr;
    GraphPtr graph_;
    std::unordered_map<Graph::EdgeId, BusId> edges_to_bus;
    std::unordered_map<Graph::EdgeId, int> edges_to_stop_pos_;
    std::unordered_map<Graph::EdgeId, size_t> edges_count;

};
