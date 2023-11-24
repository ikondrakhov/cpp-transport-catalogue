#pragma once

#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <iomanip>
#include <string_view>

#include "geo.h"

namespace transport {

    using namespace std;

    class TransportCatalogue {
    public:
        struct Route {
            std::string name;
            std::deque<std::string_view> stops;

            int CountUniqueStops() const;

            int GetStopsOnRoute() const;
        };

        struct Stop {
            std::string name;
            Coordinates coordinates;
            std::map<std::string, int> stop_to_distance;
        };

    private:
        std::map<std::string, Stop> name_to_stop_;
        std::vector<Route> routes_;
        std::map<std::string, std::set<std::string>> stop_buses_;

    public:
        void AddRoute(const Route& r);

        void AddStop(const Stop& s);

        const Route& FindRoute(const std::string_view route_name) const;

        const Stop& FindStop(const std::string& stop_name) const;

        std::set<std::string> GetBusList(const std::string& stop_name) const;

        float ComputeCurvature(const Route& r) const;
        int ComputeRouteLength(const Route& r) const;
    };
}