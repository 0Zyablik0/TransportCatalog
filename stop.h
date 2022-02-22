//
// Created by Artem Kryukov on 24.12.2021.
//


#pragma once


#include <string>
#include <utility>
#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "types.h"


namespace Stop {
    const double MAX_LATITUDE = 90, MIN_LATITUDE = -90;
    const double MAX_LONGITUDE = 180, MIN_LONGITUDE = -180;

    struct StopParams {
        std::string name_;
        double latitude_ = 0, longitude_ = 0;
    };


    class Stop {
    public:
        explicit Stop(StopParams params) : params_(std::move(params)) {};

        Stop& operator=(const Stop& ) = delete;

        [[nodiscard]] std::string GetName() const {
            return params_.name_;
        }

        [[nodiscard]] double GetLatitude() const {
            return params_.latitude_;
        }

        [[nodiscard]] double GetLongitude() const {
            return params_.longitude_;
        }

        void AddBus(BusId bus_id) {
            buses.insert(bus_id);
        }

        [[nodiscard]] const std::unordered_set<BusId> &GetBuses() const {
            return buses;
        }

        void SetLatitude(double latitude) {
            params_.latitude_ = latitude;
        }

        void SetLongitude(double longitude) {
            params_.longitude_ = longitude;
        }

        const std::unordered_map<StopId, size_t>& GetDistances(){
            return distances_;
        };

        void SetDistance(StopId stop_id, size_t distance) {
            distances_[stop_id] = distance;
        }

        [[nodiscard]] size_t GetDistance(StopId stop_id) const{
            return distances_.at(stop_id);
        }

        [[nodiscard]] bool IsReachable(StopId stop_id) const{
            return distances_.count(stop_id);
        }

        [[nodiscard]] bool IsOneEdgeConnected(StopId stop_id) const{
            return one_edge_connected_.count(stop_id);
        }
        void AddEdgeConnection(StopId stop_id){
            one_edge_connected_.insert(stop_id);
        }
        [[nodiscard]] const std::unordered_set<StopId>& OneEdgeConnectedStops() const{
            return one_edge_connected_;
        }



    private:
        StopParams params_;
        std::unordered_set<BusId> buses;
        std::unordered_map<StopId, size_t> distances_;
        std::unordered_set<StopId> one_edge_connected_;
    };

    double GeometricalDistance(const Stop &stop1, const Stop &stop2);
}

std::ostream &operator<<(std::ostream &stream, const Stop::Stop &stop);