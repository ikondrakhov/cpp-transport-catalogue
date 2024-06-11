#include "request_handler.h"
#include "json_builder.h"

using namespace std;

RequestHandler::RequestHandler(const transport::TransportCatalogue& db, const renderer::MapRenderer& renderer) : db_(db), renderer_(renderer) {
}

// Возвращает информацию о маршруте (запрос Bus)
std::map<std::string, json::Node> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
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

json::Dict RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
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

json::Dict RequestHandler::FindShortestRoute(const std::string_view from, const std::string_view to) const {
    try {
        json::Array route_items;
        double total_time = 0;
        
        auto path = db_.FindShortestRoute(from, to);
        while (!path.empty()) {
            total_time += path.front().time;
            if (path.front().type == transport::RouteItemType::WAIT) {
                route_items.push_back(json::Builder{}
                    .StartDict()
                        .Key("type").Value("Wait"s)
                        .Key("stop_name").Value(std::string(path.front().name))
                        .Key("time").Value(path.front().time)
                    .EndDict()
                .Build());
            }
            else {
                route_items.push_back(json::Builder{}
                    .StartDict()
                        .Key("type").Value("Bus"s)
                        .Key("bus").Value(std::string(path.front().name))
                        .Key("span_count").Value(path.front().span_count)
                        .Key("time").Value(path.front().time)
                    .EndDict()
                .Build());
            }
            path.pop();
        }
        return json::Builder{}
        .StartDict()
            .Key("total_time").Value(total_time)
            .Key("items").Value(route_items)
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