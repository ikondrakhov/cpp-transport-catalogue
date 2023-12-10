#pragma once

#include <iostream>
#include <deque>

#include "json.h"
#include "transport_catalogue.h"
#include "svg.h"
#include "map_renderer.h"

class JSONReader {
public:
    void ProcessRequests(std::istream& input, std::ostream& output, transport::TransportCatalogue& catalogue);

private:
    std::vector<json::Node> ProcessStatRequests(const json::Array& stat_requests,
                                                const transport::TransportCatalogue& catalogue,
                                                const renderer::MapRenderer& renderer);
    std::string ColorToString(json::Node color);
    svg::Color GetColor(json::Node color);
    void AddStopsToCatalogue(const json::Array& base_requests, transport::TransportCatalogue& catalogue);
    void AddRoutesToCatalogue(const json::Array& base_requests, transport::TransportCatalogue& catalogue);
    RenderSettings ProcessRenderSettings(const json::Node& render_settings);
};