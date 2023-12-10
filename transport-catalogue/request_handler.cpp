#include "request_handler.h"

using namespace std;

RequestHandler::RequestHandler(const transport::TransportCatalogue& db, const renderer::MapRenderer& renderer) : db_(db), renderer_(renderer) {
}

// Возвращает информацию о маршруте (запрос Bus)
std::optional<std::map<std::string, json::Node>> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    try {
        const auto& route = db_.FindRoute(bus_name);
        std::map<std::string, json::Node> result;
        result["curvature"] = db_.ComputeCurvature(route);
        result["route_length"] = db_.ComputeRouteLength(route);
        result["stop_count"] = route.GetStopsOnRoute();
        result["unique_stop_count"] = route.CountUniqueStops();
        return result;
    }
    catch (std::out_of_range& e) {
        std::map<std::string, json::Node> result;
        result["error_message"] = "not found"s;
        return result;
    }
}

std::optional<json::Dict> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    std::map<std::string, json::Node> result;
    try {
        std::vector<json::Node> buses;
        for (const auto& bus : db_.GetBusList(stop_name.data())) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }
    catch (std::out_of_range& e) {
        result["error_message"] = "not found"s;
    }
    return result;
}