//
// Created by Artem Kryukov on 28.12.2021.
//

#pragma once
#include "stop.h"
#include "bus.h"
#include <map>
#include <unordered_set>

using StopsMap = std::vector<std::shared_ptr<Stop::Stop>>;
using BusesMap = std::vector<std::shared_ptr<Bus>>;



class Database {


public:

    void AddDummyStop(const std::string &name);

    void AddBus(const std::string &name, const std::vector<std::string> &route, bool is_closed = false);


    [[nodiscard]] const BusesMap &Buses() const {
        return buses_;
    }

    [[nodiscard]] const StopsMap &Stops() const {
        return stops_;
    }

    auto GetBusList() const{
        return bus_ids;
    }

    auto GetStopList() const{
        return stop_ids;
    }

    [[nodiscard]] StopId GetStopId(const std::string &stop_name) const {
        return stop_ids.at(stop_name);
    }

    [[nodiscard]] StopId GetStopId(const std::string &stop_name) {
        StopId stop_id = -1;
        if (!IsStopInBase(stop_name)) AddDummyStop(stop_name);
        return stop_ids[stop_name];
    }

    [[nodiscard]] BusId GetBusId(const std::string &bus_name) const {
        return bus_ids.at(bus_name);
    }

    BusId GeBusId(const std::string &bus_name) {
        return bus_ids[bus_name];
    }

    [[nodiscard]] bool IsStopInBase(const std::string &stop_name) const {
        return stop_ids.count(stop_name);
    }

    [[nodiscard]] bool IsBusInBase(const std::string &bus_name) const {
        return bus_ids.count(bus_name);
    }

    [[nodiscard]] std::shared_ptr<Stop::Stop> GetStop(StopId stop_id) const {
        return stops_.at(stop_id);
    }

    [[nodiscard]] std::shared_ptr<Bus> GetBus(BusId bus_id) const {
        return buses_.at(bus_id);
    }

    [[nodiscard]] std::shared_ptr<Bus> GetBus(BusId bus_id) {
        return buses_[bus_id];
    }

    double GetStopLatitude(StopId stop_id) const{
        return stops_.at(stop_id)->GetLatitude();
    }

    double GetStopLongitude(StopId stop_id) const{
        return stops_.at(stop_id)->GetLongitude();
    }

    [[nodiscard]] auto GetStopBuses(StopId stop_id) const{
        return stops_.at(stop_id)->GetBuses();
    }

    [[nodiscard]] auto GetBusStops(BusId bus_id) const{
        return buses_.at(bus_id)->Stops();
    }

    auto OneEdgeConnectedStops(StopId stop_id) const{
        return stops_.at(stop_id)->OneEdgeConnectedStops();
    }

    bool IsNeighboringSets(const std::unordered_set<StopId>& set1, const std::unordered_set<StopId>& set2) const;


    bool IsOneEdgeConnectedSets(const std::unordered_set<StopId>& set1, const std::unordered_set<StopId>& set2) const;
private:


    StopsMap stops_;
    BusesMap buses_;
    std::map<std::string, BusId> bus_ids;
    std::map<std::string, StopId> stop_ids;

};