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

#include "geo.h"
#include "transport_catalogue.h"

namespace transport {

    using namespace std;


    int TransportCatalogue::Route::CountUniqueStops() const {
        std::set<std::string_view, std::less<>> unique_stops;
        for (const std::string_view stop : stops) {
            unique_stops.insert(stop);
        }
        return unique_stops.size();
    }

    int TransportCatalogue::Route::GetStopsOnRoute() const {
        return stops.size();
    }

    float TransportCatalogue::ComputeCurvature(const Route& r) const {
        float distance = 0;
        Coordinates previouse_coordinates = FindStop(std::string(r.stops[0])).coordinates;
        for (const std::string_view stop : r.stops) {
            Coordinates current_coordinates = FindStop(std::string(stop)).coordinates;

            distance += ComputeDistance(previouse_coordinates, current_coordinates);

            previouse_coordinates = current_coordinates;
        }
        return this->ComputeRouteLength(r) / distance;
    }

    int TransportCatalogue::ComputeRouteLength(const Route& r) const {
        int length = 0;
        for (int i = 0; i < r.stops.size() - 1; i++) {
            length += FindStop(std::string(r.stops[i])).stop_to_distance.at(std::string(r.stops[i + 1]));
        }
        return length;
    }

    void TransportCatalogue::AddRoute(Route r) {
        for (std::string_view& stop_name : r.stops) {
            stop_name = name_to_stop_[std::string(stop_name)].name;
        }
        for (std::string_view stop : r.stops) {
            stop_buses_[std::string(stop)].insert(r.name);
        }
        routes_.push_back(r);
    }

    void TransportCatalogue::AddStop(Stop s) {
        name_to_stop_[s.name].name = s.name;
        name_to_stop_[s.name].coordinates = s.coordinates;
        s.stop_to_distance.insert(name_to_stop_[s.name].stop_to_distance.begin(), name_to_stop_[s.name].stop_to_distance.end());
        name_to_stop_[s.name].stop_to_distance = s.stop_to_distance;
        for (const std::pair<std::string, int>& distance_to_stop : s.stop_to_distance) {
            if (name_to_stop_[distance_to_stop.first].stop_to_distance.find(s.name) == name_to_stop_[distance_to_stop.first].stop_to_distance.end()) {
                name_to_stop_[distance_to_stop.first].stop_to_distance[s.name] = distance_to_stop.second;
            }
        }
    }

    const TransportCatalogue::Route& TransportCatalogue::FindRoute(const std::string_view route_name) const {
        const auto& r = find_if(routes_.begin(), routes_.end(),
            [&route_name](const Route& r) {
                return r.name == route_name;
            });
        if (r == routes_.end()) {
            throw std::out_of_range("No route with name "s + std::string(route_name) + " exists"s);
        }
        return *r;
    }

    const TransportCatalogue::Stop& TransportCatalogue::FindStop(const std::string& stop_name) const {
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
}