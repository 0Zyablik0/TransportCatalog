//
// Created by Artem Kryukov on 24.12.2021.
//

#include "bus.h"

#include <utility>

void Bus::AddStop(StopId stop_id, std::shared_ptr<Stop::Stop> stop){
    unique_stops_[stop_id] = std::move(stop);
    stop_times_[stop_id] += 1;
    stops_.push_back(stop_id);
}

Distances Bus::Length() const {
    size_t distance = 0;
    double geometrical_distance = 0;
    for (size_t i = 0; i + 1 < stops_.size(); i++) {
        distance += GetDistance(stops_[i], stops_[i + 1]);
        geometrical_distance  += GetGeometricalDistance(stops_[i], stops_[i + 1]);
    }
    if (closed_route_)
        return {.distance_ = distance, .geometrical_distance = geometrical_distance};
    for (size_t i = 0; i + 1 < stops_.size(); i++) {
        distance += GetDistance(stops_[i + 1], stops_[i]);
    }
    return {.distance_ = distance, .geometrical_distance = 2 * geometrical_distance};
}



