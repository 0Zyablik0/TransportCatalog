//
// Created by Artem Kryukov on 31.12.2021.
//

#include "coordinate_table.h"

void CoordinateTable::UpdateTable() {
    interpolator_->Update();
    for (const auto &[_, stop_id]: database_->GetStopList()) {
        sorted_by_longitudes_[interpolator_->GetLongitude(stop_id)].insert(stop_id);
        sorted_by_latitudes_[interpolator_->GetLatitude(stop_id)].insert(stop_id);
    }
    OptimizePositions();
}


void CoordinateTable::OptimizePositions() {
    OptimizeLongitudes();
    OptimizeLatitudes();
}


void CoordinateTable::SetStopLongitude(StopId stop_id) {
    int count = 0;
    for (const auto& stop:database_->OneEdgeConnectedStops(stop_id)){
        if (longitudes_.count(interpolator_->GetLongitude(stop))){
            count = std::max(count, longitudes_[interpolator_->GetLongitude(stop)] + 1);
        }
    }
    longitudes_[interpolator_->GetLongitude(stop_id)] = count;
    max_longitude_idx_ = max_longitude_idx_ > count + 1 ? max_longitude_idx_ : count + 1;
}

void CoordinateTable::SetStopLatitude(StopId stop_id) {
    int count = 0;
    for (const auto& stop:database_->OneEdgeConnectedStops(stop_id)){
        if (latitudes_.count(interpolator_->GetLatitude(stop))){
            count = std::max(count, latitudes_[interpolator_->GetLatitude(stop)] + 1);
        }
    }
    latitudes_[interpolator_->GetLatitude(stop_id)] = count;
    max_latitude_idx_ = max_latitude_idx_ > count + 1 ? max_latitude_idx_ : count + 1;

}

void CoordinateTable::OptimizeLongitudes() {
    int count = 0;
    auto it_current = sorted_by_longitudes_.begin();
    auto it_next =  sorted_by_longitudes_.begin();
    while(it_current != sorted_by_longitudes_.end()) {
        for (const auto& stop : it_current->second){
            SetStopLongitude(stop);
        }
        it_current ++;
    }
}



void CoordinateTable::OptimizeLatitudes(){
    int count = 0;
    auto it_current = sorted_by_latitudes_.begin();
    auto it_next =  sorted_by_latitudes_.begin();
    while(it_current != sorted_by_latitudes_.end()) {
        for (const auto& stop : it_current->second){
            SetStopLatitude(stop);
        }
        it_current ++;
    }

}