#include "transport_router.h"

#include <set>
#include <stdexcept>
#include <queue>
#include <stack>
#include <iostream>
#include <chrono>

namespace graph {

    using namespace transport;

    void TransportRouter::InitRouter(const std::map<std::string, transport::Stop>& name_to_stop, const std::set<transport::Route>& routes,
                                    transport::RoutingSettings routing_settings) {
        routing_settings_ = routing_settings;
        graph_ = DirectedWeightedGraph<RouteItem>(name_to_stop.size() * 2);

        size_t id = 0;
        for (const auto& [stop_name, stop] : name_to_stop) {
            stop_ids[stop_name] = id;
            graph_.AddEdge({ id, id + 1, {RouteItemType::WAIT, stop_name, 1, routing_settings_.bus_wait_time} });
            id += 2;
        }

        for (const auto& route : routes) {
            for (int i = 0; i < route.stops.size(); i++) {
                double current_path_length = 0;
                for (int j = i + 1; j < route.stops.size(); j++) {
                    current_path_length += route.stops[j - 1]->stop_to_distance.at(route.stops[j]->name);
                    graph_.AddEdge({ stop_ids.at(route.stops[i]->name) + 1, stop_ids.at(route.stops[j]->name), 
                        {RouteItemType::BUS, route.name, j - i, current_path_length / routing_settings.bus_velocity } });
                }
            }
        }
        router = std::make_unique<Router<transport::RouteItem>>(graph_);
    }

    std::queue<transport::RouteItem> TransportRouter::FindShortestRoute(const std::string_view from, const std::string_view to) const {
        if (from == to) {
            return {};
        }

        auto route = router->BuildRoute(stop_ids.at(from), stop_ids.at(to));
        std::queue<transport::RouteItem> result;

        if (route.has_value()) {
            for (size_t edge : route.value().edges) {
                result.push(graph_.GetEdge(edge).weight);
            }
        }
        else {
            throw std::out_of_range("not found");
        }

        return result;
    }
}