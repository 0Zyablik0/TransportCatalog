//
// Created by Artem Kryukov on 24.12.2021.
//

#pragma once

#include <vector>
#include <memory>
#include <utility>
#include "stop.h"
#include "unordered_map"
#include <tuple>
#include "types.h"


struct Distances {
    size_t distance_;
    double geometrical_distance;
};

class Bus {
public:
    Bus(std::string name, bool closed) :
            name_(std::move(name)),
            closed_route_(closed) {};

    void AddStop(StopId stop_id, std::shared_ptr<Stop::Stop> stop);

    [[nodiscard]] std::string GetName() const {
        return name_;
    }


    [[nodiscard]] Distances Length() const;

    [[nodiscard]] size_t UniqueStops() const {
        return unique_stops_.size();
    }

    [[nodiscard]] size_t TotalStops() const {
        if (closed_route_)
            return stops_.size();
        return 2 * stops_.size() - 1;
    }

    [[nodiscard]] bool IsClosed() const {
        return closed_route_;
    }

    const std::vector<StopId> &Stops() const {
        return stops_;
    }

    [[nodiscard]] const std::unordered_map<StopId, std::shared_ptr<Stop::Stop>> &Unique() const {
        return unique_stops_;
    }

    [[nodiscard]] size_t GetDistance(StopId stop1, StopId stop2) const {
        return unique_stops_.at(stop1)->GetDistance(stop2);
    }

    [[nodiscard]] double GetGeometricalDistance(StopId stop1, StopId stop2) const {
        return Stop::GeometricalDistance(*unique_stops_.at(stop1), *unique_stops_.at(stop2));
    }

    [[nodiscard]] StopId GetStop(size_t i) const {
        return stops_.at(i);
    }

    [[nodiscard]] int GetTimes(StopId stop_id) const {
        return stop_times_.at(stop_id);
    }

    [[nodiscard]] bool IsEndStop(StopId stop_id) const {
        return stop_id == stops_.front() || (!closed_route_ && stop_id == stops_.back());
    }

private:
    std::string name_;
    std::vector<StopId> stops_;
    std::unordered_map<StopId, std::shared_ptr<Stop::Stop>> unique_stops_;
    std::unordered_map<StopId, int> stop_times_;
    bool closed_route_ = false;
};