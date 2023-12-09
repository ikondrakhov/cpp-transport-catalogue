#include "json_reader.h"
#include "map_renderer.h"
#include "geo.h"
#include "svg.h"
#include "request_handler.h"

#include <sstream>

using namespace std;


std::string ColorToString(json::Node color) {
    std::ostringstream str;
    if (color.IsString()) {
        str << color.AsString();
    }
    else if (color.IsArray()) {
        if (color.AsArray().size() == 3) {
            str << "rgb(" + std::to_string(color.AsArray()[0].AsInt()) + ","
                + std::to_string(color.AsArray()[1].AsInt()) + ","
                + std::to_string(color.AsArray()[2].AsInt()) + ")";
        }
        else if (color.AsArray().size() == 4) {
            str << "rgba("s << color.AsArray()[0].AsInt() << ","s
                << color.AsArray()[1].AsInt() << ","s
                << color.AsArray()[2].AsInt() << ","s
                << color.AsArray()[3].AsDouble() << ")"s;
        }
    }
    return str.str();
}

void ProcessRequests(std::istream& input, std::ostream& output, transport::TransportCatalogue& catalogue) {
    json::Node requests = json::Load(input).GetRoot();

    for (const auto& base_requests : requests.AsMap().at("base_requests").AsArray()) {
        const auto& request = base_requests.AsMap();
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
    
    for (const auto& base_requests : requests.AsMap().at("base_requests").AsArray()) {
        const auto& request = base_requests.AsMap();
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

    json::Node render_params = requests.AsMap().at("render_settings");
    RenderSettings render_settings = {
        render_params.AsMap().at("width").AsDouble(),
        render_params.AsMap().at("height").AsDouble(),
        render_params.AsMap().at("padding").AsDouble(),
        render_params.AsMap().at("line_width").AsDouble(),
        render_params.AsMap().at("stop_radius").AsDouble(),
        render_params.AsMap().at("bus_label_font_size").AsInt(),
        { render_params.AsMap().at("bus_label_offset").AsArray()[0].AsDouble(), render_params.AsMap().at("bus_label_offset").AsArray()[1].AsDouble() },
        render_params.AsMap().at("stop_label_font_size").AsInt(),
        { render_params.AsMap().at("stop_label_offset").AsArray()[0].AsDouble(), render_params.AsMap().at("stop_label_offset").AsArray()[1].AsDouble() },
        render_params.AsMap().at("underlayer_color"),
        render_params.AsMap().at("underlayer_width").AsDouble(),
        render_params.AsMap().at("color_palette").AsArray()
    };

    std::vector<json::Node> responses;

    renderer::MapRenderer renderer {render_settings};
    RequestHandler request_handler(catalogue, renderer);

    for (const auto& request : requests.AsMap().at("stat_requests").AsArray()) {
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
            auto map = request_handler.RenderMap();
            map.Render(result_map);
            result.value()["map"] = result_map.str();
        }

        if (result.has_value()) {
            result.value()["request_id"] = request_id;
        }
        responses.push_back(result.value());
    }

    json::Print(json::Document(responses), output);
}