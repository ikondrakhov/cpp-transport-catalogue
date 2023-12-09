#include "domain.h"

#include <set>

namespace transport {

    int Route::CountUniqueStops() const {
        std::set<std::string_view, std::less<>> unique_stops;
        for (const std::string_view stop : stops) {
            unique_stops.insert(stop);
        }
        return unique_stops.size();
    }

    int Route::GetStopsOnRoute() const {
        return stops.size();
    }
}