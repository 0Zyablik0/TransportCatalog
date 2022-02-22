//
// Created by Artem Kryukov on 28.12.2021.
//

#include "database.h"
#include "bus.h"
#include <memory>


void Database::AddBus(const std::string &name, const std::vector<std::string> &route, bool is_closed) {
    class Bus bus(name, is_closed);
    BusId bus_id = -1;
    bus_id = (BusId) bus_ids.size();
    StopId previous_stop = -1;
    for (size_t i = 0; i < route.size(); i++) {
        StopId stop_id = -1;
        if (!stop_ids.count(route[i])) {
            AddDummyStop(route[i]);
        }
        stop_id = stop_ids[route[i]];
        if (previous_stop >= 0){
            stops_[stop_id]->AddEdgeConnection(previous_stop);
            stops_[previous_stop]->AddEdgeConnection(stop_id);
        }
        bus.AddStop(stop_id, stops_[stop_id]);
        stops_[stop_id]->AddBus(bus_id);
        previous_stop = stop_id;
    }
    bus_ids[bus.GetName()] = bus_id;
    buses_.push_back(std::make_shared<Bus>(bus));
}

void Database::AddDummyStop(const std::string &name) {
    Stop::StopParams params = {.name_ =  name, .latitude_ = -1, .longitude_ = -1};
    StopId stop_id = (StopId) stop_ids.size();
    stop_ids[name] = stop_id;
    stops_.push_back(std::make_shared<Stop::Stop>(params));
}


bool Database::IsNeighboringSets(const std::unordered_set<StopId> &set1, const std::unordered_set<StopId> &set2) const {
    for (const auto &stop1: set1) {
        for (const auto &stop2: set2) {
            if (!stops_.at(stop1)->IsReachable(stop2) and stop1 != stop2)
                return false;
        }
    }
    return true;
}

bool Database::IsOneEdgeConnectedSets(const std::unordered_set<StopId> &set1,
                                      const std::unordered_set<StopId> &set2) const {
    for (const auto &stop1: set1) {
        for (const auto &stop2: set2) {
            if (!stops_.at(stop1)->IsOneEdgeConnected(stop2) and stop1 != stop2)
                return false;
        }
    }
    return true;
}