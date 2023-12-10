#include "request_handler.h"
#include "json_builder.h"

using namespace std;

RequestHandler::RequestHandler(const transport::TransportCatalogue& db, const renderer::MapRenderer& renderer) : db_(db), renderer_(renderer) {
}

// Возвращает информацию о маршруте (запрос Bus)
std::optional<std::map<std::string, json::Node>> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    try {
        const auto& route = db_.FindRoute(bus_name);
        return json::Builder{}
                .StartDict()
                    .Key("curvature").Value(db_.ComputeCurvature(route))
                    .Key("route_length").Value(db_.ComputeRouteLength(route))
                    .Key("stop_count").Value(route.GetStopsOnRoute())
                    .Key("unique_stop_count").Value(route.CountUniqueStops())
                .EndDict()
                .Build()
                .AsDict();
    }
    catch (std::out_of_range& e) {
        return json::Builder{}
                .StartDict()
                    .Key("error_message").Value("not found"s)
                .EndDict()
                .Build()
                .AsDict();
    }
}

std::optional<json::Dict> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    try {
        json::Array buses;
        for (const auto& bus : db_.GetBusList(stop_name.data())) {
            buses.push_back(bus);
        }
        return json::Builder{}
            .StartDict()
                .Key("buses").Value(buses)
            .EndDict()
            .Build()
            .AsDict();
    }
    catch (std::out_of_range& e) {
        return json::Builder{}
                .StartDict()
                    .Key("error_message").Value("not found"s)
                .EndDict()
                .Build()
                .AsDict();
    }
}