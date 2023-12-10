#include "json_reader.h"
#include "map_renderer.h"
#include "geo.h"
#include "svg.h"
#include "request_handler.h"

#include <sstream>

using namespace std;

svg::Color JSONReader::GetColor(json::Node color) {
    if(color.IsString()) {
        return color.AsString();
    } else if (color.IsArray()) {
        if(color.AsArray().size() == 3) {
            return svg::Rgb(color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt());
        } else {
            return svg::Rgba(color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(),
                         color.AsArray()[2].AsInt(), color.AsArray()[3].AsDouble());
        }
    }
    return svg::Color{};
}

void JSONReader::AddStopsToCatalogue(const json::Array& base_requests, transport::TransportCatalogue& catalogue) {
    for (const auto& r : base_requests) {
        const auto& request = r.AsMap();
        if (request.at("type").AsString() == "Stop") {
            std::map<std::string, int> stop_to_distance;
            for (const auto& [street_name, distance] : request.at("road_distances").AsMap()) {
                stop_to_distance[street_name] = distance.AsInt();
            }
            catalogue.AddStop({ request.at("name").AsString(),
                                {request.at("latitude").AsDouble(), request.at("longitude").AsDouble()},
                                stop_to_distance });
        }
    }
}

void JSONReader::AddRoutesToCatalogue(const json::Array& base_requests, transport::TransportCatalogue& catalogue) {
    for (const auto& r : base_requests) {
        const auto& request = r.AsMap();
        if (request.at("type").AsString() == "Bus") {
            std::deque<std::string> stops;
            for (const auto& stop : request.at("stops").AsArray()) {
                stops.push_back(stop.AsString());
            }

            if (!request.at("is_roundtrip").AsBool()) {
                std::vector<std::string_view> copy(stops.begin(), stops.end() - 1);
                std::reverse(copy.begin(), copy.end());
                stops.insert(stops.end(), copy.begin(), copy.end());
            }
            transport::Route r = { request.at("name").AsString(), stops, request.at("is_roundtrip").AsBool() };
            catalogue.AddRoute(r);
        }
    }
}

RenderSettings JSONReader::ProcessRenderSettings(const json::Node& render_settings) {
    std::vector<svg::Color> color_palette;
    for(const auto& color: render_settings.AsMap().at("color_palette").AsArray()) {
        color_palette.push_back(GetColor(color));
    }
    return {
        render_settings.AsMap().at("width").AsDouble(),
        render_settings.AsMap().at("height").AsDouble(),
        render_settings.AsMap().at("padding").AsDouble(),
        render_settings.AsMap().at("line_width").AsDouble(),
        render_settings.AsMap().at("stop_radius").AsDouble(),
        render_settings.AsMap().at("bus_label_font_size").AsInt(),
        { 
            render_settings.AsMap().at("bus_label_offset").AsArray()[0].AsDouble(),
            render_settings.AsMap().at("bus_label_offset").AsArray()[1].AsDouble() 
        },
        render_settings.AsMap().at("stop_label_font_size").AsInt(),
        { 
            render_settings.AsMap().at("stop_label_offset").AsArray()[0].AsDouble(),
            render_settings.AsMap().at("stop_label_offset").AsArray()[1].AsDouble() 
        },
        GetColor(render_settings.AsMap().at("underlayer_color")),
        render_settings.AsMap().at("underlayer_width").AsDouble(),
        color_palette
    };
}

std::vector<json::Node> JSONReader::ProcessStatRequests(const json::Array& stat_requests, const transport::TransportCatalogue& catalogue,
                                            const renderer::MapRenderer& renderer) {
    RequestHandler request_handler(catalogue, renderer);
    std::vector<json::Node> responses;

    for (const auto& request : stat_requests) {
        const std::string& type = request.AsMap().at("type").AsString();
        int request_id = request.AsMap().at("id").AsInt();
        std::optional<json::Dict> result = json::Dict{};

        if (type == "Bus") {
            result = request_handler.GetBusStat(request.AsMap().at("name").AsString());
        }
        else if (type == "Stop") {
            result = request_handler.GetBusesByStop(request.AsMap().at("name").AsString());
        }
        else if (type == "Map") {
            std::ostringstream result_map;
            auto map = renderer.RenderMap(catalogue.GetRouteList(), catalogue.GetStopsCoordinates());
            map.Render(result_map);
            result.value()["map"] = result_map.str();
        }

        if (result.has_value()) {
            result.value()["request_id"] = request_id;
        }
        responses.push_back(result.value());
    }
    return responses;
}

void JSONReader::ProcessRequests(std::istream& input, std::ostream& output, transport::TransportCatalogue& catalogue) {
    json::Node requests = json::Load(input).GetRoot();

    AddStopsToCatalogue(requests.AsMap().at("base_requests").AsArray(), catalogue);
    AddRoutesToCatalogue(requests.AsMap().at("base_requests").AsArray(), catalogue);

    renderer::MapRenderer renderer {ProcessRenderSettings(requests.AsMap().at("render_settings"))};
    std::vector<json::Node> responses = ProcessStatRequests(requests.AsMap().at("stat_requests").AsArray(), catalogue, renderer);

    json::Print(json::Document(responses), output);
}