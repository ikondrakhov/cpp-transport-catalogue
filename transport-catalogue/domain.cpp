#include "domain.h"

#include <set>

namespace transport {

    int Route::CountUniqueStops() const {
        std::set<std::string_view, std::less<>> unique_stops;
        for (const Stop* stop : stops) {
            unique_stops.insert(stop->name);
        }
        return unique_stops.size();
    }

    int Route::GetStopsOnRoute() const {
        return stops.size();
    }

    bool operator < (const RouteItem& ril, const RouteItem& rir) {
        return ril.time < rir.time;
    }

    bool operator > (const RouteItem& ril, const RouteItem& rir) {
        return rir < ril;
    }

    RouteItem operator+(const RouteItem& ril, const RouteItem& rir) {
        return { RouteItemType::BUS, ril.name, ril.span_count + rir.span_count, ril.time + rir.time };
    }
}