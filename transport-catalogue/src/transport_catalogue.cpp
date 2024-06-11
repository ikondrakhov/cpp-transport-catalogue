#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <queue>
#include <limits>

#include "geo.h"
#include "transport_catalogue.h"
#include "graph.h"

namespace transport {

    using namespace std;

    float TransportCatalogue::ComputeCurvature(const Route& r) const {
        float distance = 0;
        Coordinates previouse_coordinates = r.stops[0]->coordinates;
        for (const Stop* stop : r.stops) {
            Coordinates current_coordinates = stop->coordinates;

            distance += ComputeDistance(previouse_coordinates, current_coordinates);

            previouse_coordinates = current_coordinates;
        }
        return this->ComputeRouteLength(r) / distance;
    }

    int TransportCatalogue::ComputeRouteLength(const Route& r) const {
        int length = 0;
        for (size_t i = 0; i < r.stops.size() - 1; i++) {
            length += r.stops[i]->stop_to_distance.at(r.stops[i + 1]->name);
        }
        return length;
    }

    void TransportCatalogue::AddRoute(const Route& r) {
        routes_.insert(r);
        for (const transport::Stop* stop : r.stops) {
            stop_buses_[stop->name].insert(r.name);
        }
    }

    void TransportCatalogue::AddStop(const Stop& s) {
        name_to_stop_[s.name].name = s.name;
        name_to_stop_[s.name].coordinates = s.coordinates;

        // add distance from stop s to other stops
        for (const auto& [stop_name, distance] : s.stop_to_distance) {
            name_to_stop_[s.name].stop_to_distance[stop_name] = distance;
        }

        // add distance from other stops to stop s
        for (const auto& [stop_name, distance] : s.stop_to_distance) {
            if (name_to_stop_[stop_name].stop_to_distance.find(s.name) == name_to_stop_[stop_name].stop_to_distance.end()) {
                name_to_stop_[stop_name].stop_to_distance[s.name] = distance;
            }
        }
    }

    void TransportCatalogue::InitRouter(transport::RoutingSettings routing_settings) {
        transport_router_.InitRouter(this->name_to_stop_, this->routes_, routing_settings);
    }

    const Route& TransportCatalogue::FindRoute(const std::string_view route_name) const {
        const auto& r = find_if(routes_.begin(), routes_.end(),
            [&route_name](const Route& r) {
                return r.name == route_name;
            });
        if (r == routes_.end()) {
            throw std::out_of_range("No route with name "s + std::string(route_name) + " exists"s);
        }
        return *r;
    }

    const Stop& TransportCatalogue::FindStop(const std::string& stop_name) const {
        if (name_to_stop_.find(stop_name) == name_to_stop_.end()) {
            throw std::out_of_range("No stop with name "s + stop_name + " exists"s);
        }
        return name_to_stop_.at(stop_name);
    }

    std::set<std::string> TransportCatalogue::GetBusList(const std::string& stop_name) const {
        if (name_to_stop_.find(stop_name) != name_to_stop_.end() && stop_buses_.find(stop_name) == stop_buses_.end()) {
            return {};
        }
        return stop_buses_.at(stop_name);
    }

    const std::set<Route>& TransportCatalogue::GetRouteList() const {
        return routes_;
    }


    std::map<std::string_view, Coordinates> TransportCatalogue::GetStopsCoordinates() const {
        std::map<std::string_view, Coordinates> stop_coordiantes;
        for (const auto& [stop_name, stop] : name_to_stop_) {
            stop_coordiantes[stop_name] = stop.coordinates;
        }
        return stop_coordiantes;
    }

    std::queue<transport::RouteItem> TransportCatalogue::FindShortestRoute(const std::string_view from, const std::string_view to) const {
        return transport_router_.FindShortestRoute(from, to);
    }
}