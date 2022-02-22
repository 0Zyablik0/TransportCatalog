//
// Created by Artem Kryukov on 29.12.2021.
//
#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "database.h"
#include "interpolator.h"


class CoordinateTable {
public:

    CoordinateTable(std::shared_ptr<Database> database):
    database_(database),
    interpolator_(std::make_unique<Interpolator>(database))
    {};

    [[nodiscard]] int GetLatitudePos(StopId stop_id) const {
        return latitudes_.at(interpolator_->GetLatitude(stop_id));
    }

    [[nodiscard]] int GetLongitudePos(StopId stop_id) const {
        return longitudes_.at(interpolator_->GetLongitude(stop_id));
    }

    size_t MaxLatitude() {
        return max_latitude_idx_;
    }

    size_t MaxLongitude() {
        return max_longitude_idx_;
    }

    void UpdateTable();



private:
    void SetStopLongitude(StopId stop_id);
    void SetStopLatitude(StopId stop_id);
    void OptimizePositions();
    void OptimizeLongitudes();
    void OptimizeLatitudes();


    std::shared_ptr<Database> database_;
    std::unique_ptr<Interpolator> interpolator_;
    std::unordered_map<double, int> latitudes_;
    std::unordered_map<double, int> longitudes_;
    std::map<double, std::unordered_set<StopId>> sorted_by_latitudes_;
    std::map<double, std::unordered_set<StopId>> sorted_by_longitudes_;
    size_t max_latitude_idx_ = 0, max_longitude_idx_ = 0;
};