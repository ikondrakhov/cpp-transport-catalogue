#pragma once

#include <string>
#include <deque>
#include <map>

#include "geo.h"

namespace transport {
    struct Stop {
        std::string name;
        Coordinates coordinates;
        std::map<std::string, int> stop_to_distance;
    };

    struct Route {
        std::string name;
        std::deque<const Stop*> stops;
        bool is_roundtrip = false;

        int CountUniqueStops() const;

        int GetStopsOnRoute() const;

        bool operator<(const Route& rhs) const {
            return name < rhs.name;
        }
    };

    struct RoutingSettings {
        double bus_wait_time;
        double bus_velocity;
    };

    enum class RouteItemType {
        WAIT,
        BUS
    };

    struct RouteItem {
        RouteItemType type;
        std::string_view name;
        int span_count;
        double time = 0;
    };

    bool operator < (const RouteItem& ril, const RouteItem& rir);

    bool operator > (const RouteItem& ril, const RouteItem& rir);

    RouteItem operator+(const RouteItem& ril, const RouteItem& rir);
}