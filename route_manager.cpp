//
// Created by Artem Kryukov on 28.12.2021.
//

#include "route_manager.h"


void GraphManager::AddStopEdge(StopId stop_id) {
    auto edge_id = graph_->AddEdge(
            {.from = GetArrivalVertex(stop_id),
                    .to = GetDepartureVertex(stop_id),
                    .weight = 1.0 * wait_time_});
    edges_to_bus[edge_id] = -1;
}

double GraphManager::CalculateWeight(double distance) const {
    return 0.06 * distance/bus_velocity_;
}

Graph::EdgeId GraphManager::AddEdge(StopId stop1, StopId stop2, BusId bus_id, double distance) {
    auto edge_id = graph_->AddEdge(
            {.from = GetDepartureVertex(stop1),
                    .to = GetArrivalVertex(stop2),
                    .weight = CalculateWeight(distance)});
    edges_to_bus[edge_id] = bus_id;
    return edge_id;
}

void GraphManager::AddBusEdges(BusId bus_id) {
    const auto& buses = database_->Buses();
    const auto& stops = buses[bus_id]->Stops();
    if (stops.empty()) return;
    for (int i = 0; i < stops.size(); i++) {
        StopId to = stops.at(i);
        double front_weight = 0;
        double back_weight = 0;
        for (int j = i + 1; j < stops.size(); j++) {
            front_weight += buses.at(bus_id)->GetDistance(stops.at(j - 1), stops.at(j));
            auto edge_id = AddEdge(stops.at(i), stops.at(j), bus_id, front_weight);
            edges_count[edge_id] = j - i;
            edges_to_stop_pos_[edge_id] = i;
            if (not buses[bus_id]->IsClosed()) {
                back_weight += buses[bus_id]->GetDistance(stops.at(j), stops.at(j - 1));
                edge_id = AddEdge(stops.at(j), stops.at(i), bus_id, back_weight);
                edges_count[edge_id] = j - i;
                edges_to_stop_pos_[edge_id] = -j;
            }
        }
    }
}

void GraphManager::BuildGraph() {
    Validate();
    const auto& stops = database_->Stops();
    const auto& buses = database_->Buses();
    size_t num_vertices = stops.size() * 2;
    InitializeGraph(num_vertices);
    Graph::Edge<double> edge = {0, 0, 0};
    for (StopId i = 0; i < (StopId) stops.size(); i++) {
        AddStopEdge(i);
    }
    for (BusId bus_id = 0; bus_id < (BusId) buses.size(); bus_id++) {
        AddBusEdges(bus_id);
    }
}
