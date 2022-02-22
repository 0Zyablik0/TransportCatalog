//
// Created by Artem Kryukov on 31.12.2021.
//

#pragma once

#include <list>
#include "database.h"


class Interpolator {
public:
    explicit Interpolator(std::shared_ptr<Database> database) : database_(std::move(database)) {}


    double GetLatitude(StopId stop_id){
        return latitudes_[stop_id];
    }

    double GetLongitude(StopId stop_id){
        return longitudes_[stop_id];
    }

    void Update();

private:
    [[nodiscard]] bool IsReferenceStop(StopId stop_id) const;

    void InterpolateRoute(BusId bus_id);
    void AddReferenceStop(StopId stop_id);

    std::shared_ptr<Database> database_;
    std::unordered_map<StopId, double> latitudes_;
    std::unordered_map<StopId, double> longitudes_;
    std::unordered_set<StopId> reference_stops_;

};

