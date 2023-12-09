#pragma once

#include <string>
#include <deque>
#include <map>

#include "geo.h"

namespace transport {
    struct Route {
        std::string name;
        std::deque<std::string> stops;
        bool is_roundtrip = false;

        int CountUniqueStops() const;

        int GetStopsOnRoute() const;

        bool operator<(const Route& rhs) const {
            return name < rhs.name;
        }
    };

    struct Stop {
        std::string name;
        Coordinates coordinates;
        std::map<std::string, int> stop_to_distance;
    };
}