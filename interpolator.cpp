//
// Created by Artem Kryukov on 31.12.2021.
//

#include "interpolator.h"

bool Interpolator::IsReferenceStop(StopId stop_id) const {
    auto buses = database_->GetStopBuses(stop_id);
    if (buses.size() > 1) return true;
    if (buses.empty()) return true;
    auto bus = *buses.begin();
    auto stops = database_->GetBusStops(bus);
    if (stops.front() == stop_id or stops.back() == stop_id)
        return true;
    if(database_->GetBus(bus)->GetTimes(stop_id) > 2)
        return true;
    if (database_->GetBus(bus)->GetTimes(stop_id) == 2)
        return !database_->GetBus(bus)->IsClosed();
    return false;
}

void Interpolator::AddReferenceStop(StopId stop_id) {
    latitudes_[stop_id] = database_->GetStopLatitude(stop_id);
    longitudes_[stop_id] = database_->GetStopLongitude(stop_id);
    reference_stops_.insert(stop_id);
}

void Interpolator::InterpolateRoute(BusId bus_id) {
    auto stops = database_->GetBusStops(bus_id);
    int i = 0;
    while (i < stops.size()){
        int j = i + 1;
        std::vector<StopId> not_reference_stops;
        while (j < stops.size() and !reference_stops_.count(stops[j])){
            not_reference_stops.push_back(stops[j]);
            j ++;
        }
        if (j == stops.size()) break;
        double lon_step = (GetLongitude(stops[j]) - GetLongitude(stops[i]))/(j - i);
        double lat_step = (GetLatitude(stops[j]) - GetLatitude(stops[i]))/(j - i);
        int k = 1;
        for (auto stop_id : not_reference_stops){
            latitudes_[stop_id] = GetLatitude(stops[i])  + k * lat_step;
            longitudes_[stop_id] = GetLongitude(stops[i]) + k*lon_step;
            k ++;
        }
        i = j;
    }
}

void Interpolator::Update() {
    {
        latitudes_.clear();
        longitudes_.clear();
        reference_stops_.clear();
        for (const auto&[_, stop_id]: database_->GetStopList()) {
            if (IsReferenceStop(stop_id)) {
                AddReferenceStop(stop_id);
            }
        }
        for (const auto &bus: database_->GetBusList()) {
            InterpolateRoute(bus.second);
        }

    }
}