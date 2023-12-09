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


svg::Document RequestHandler::RenderMap() const {
    using namespace svg;

    std::vector<Coordinates> coordinates;
    std::set<std::string_view> stops;
    // get stops whitch are used in routes
    for (const auto& route : db_.GetRouteList()) {
        for (const auto& stop_name : route.stops) {
            coordinates.push_back(db_.FindStop(std::string(stop_name)).coordinates);
            stops.insert(stop_name);
        }
    }

    SphereProjector projector(coordinates.begin(), coordinates.end(), renderer_.render_settings.width, renderer_.render_settings.height, renderer_.render_settings.padding);

    std::vector<json::Node> color_palette = renderer_.render_settings.color_palette;
    std::vector<svg::Color> colors;
    for (json::Node& color : color_palette) {
        colors.push_back(ColorToString(color));
    }

    Document map;

    // Add routes to map
    int color_num = 0;
    for (const auto& route : db_.GetRouteList()) {
        if (route.stops.size() > 1) {
            Polyline route_line = Polyline()
                .SetFillColor("none")
                .SetStrokeColor(colors[color_num])
                .SetStrokeWidth(renderer_.render_settings.line_width)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND);
            for (const auto& stop_name : route.stops) {
                Point p = projector(db_.FindStop(std::string(stop_name)).coordinates);
                route_line.AddPoint(p);
            }
            map.Add(route_line);
            color_num = (color_num + 1) % colors.size();
        }
    }

    // Add route names
    color_num = 0;
    for (const auto& route : db_.GetRouteList()) {
        if (route.stops.size() > 1) {
            Text route_name = Text()
                .SetFillColor(colors[color_num])
                .SetPosition(projector(db_.FindStop(std::string(route.stops[0])).coordinates))
                .SetOffset(renderer_.render_settings.bus_label_offset)
                .SetFontSize(renderer_.render_settings.bus_label_font_size)
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(route.name);
            Text under_route_name = Text()
                .SetFillColor(ColorToString(renderer_.render_settings.underlayer_color))
                .SetStrokeColor(ColorToString(renderer_.render_settings.underlayer_color))
                .SetStrokeWidth(renderer_.render_settings.underlayer_width)
                .SetPosition(projector(db_.FindStop(std::string(route.stops[0])).coordinates))
                .SetOffset(renderer_.render_settings.bus_label_offset)
                .SetFontSize(renderer_.render_settings.bus_label_font_size)
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(route.name)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND);
            map.Add(under_route_name);
            map.Add(route_name);
            if (!route.is_roundtrip && (route.stops[0] != route.stops[route.stops.size() / 2])) {
                route_name = Text()
                    .SetFillColor(colors[color_num])
                    .SetPosition(projector(db_.FindStop(std::string(route.stops[route.stops.size() / 2])).coordinates))
                    .SetOffset(renderer_.render_settings.bus_label_offset)
                    .SetFontSize(renderer_.render_settings.bus_label_font_size)
                    .SetFontFamily("Verdana")
                    .SetFontWeight("bold")
                    .SetData(route.name);
                under_route_name = Text()
                    .SetFillColor(ColorToString(renderer_.render_settings.underlayer_color))
                    .SetStrokeColor(ColorToString(renderer_.render_settings.underlayer_color))
                    .SetStrokeWidth(renderer_.render_settings.underlayer_width)
                    .SetPosition(projector(db_.FindStop(std::string(route.stops[route.stops.size() / 2])).coordinates))
                    .SetOffset(renderer_.render_settings.bus_label_offset)
                    .SetFontSize(renderer_.render_settings.bus_label_font_size)
                    .SetFontFamily("Verdana")
                    .SetFontWeight("bold")
                    .SetData(route.name)
                    .SetStrokeLineCap(StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(StrokeLineJoin::ROUND);
                map.Add(under_route_name);
                map.Add(route_name);
            }
            color_num = (color_num + 1) % colors.size();
        }
    }

    // Add stop circles
    for (std::string_view stop_name : stops) {
        const auto& stop = db_.FindStop(std::string(stop_name));
        map.Add(Circle()
            .SetCenter(projector(stop.coordinates))
            .SetRadius(renderer_.render_settings.stop_radius)
            .SetFillColor("white"));
    }

    // Add stop names
    for (std::string_view stop_name : stops) {
        const auto& stop = db_.FindStop(std::string(stop_name));

        map.Add(Text()
            .SetPosition(projector(stop.coordinates))
            .SetOffset(renderer_.render_settings.stop_label_offset)
            .SetFontSize(renderer_.render_settings.stop_label_font_size)
            .SetFontFamily("Verdana")
            .SetData(stop.name)
            .SetFillColor(ColorToString(renderer_.render_settings.underlayer_color))
            .SetStrokeColor(ColorToString(renderer_.render_settings.underlayer_color))
            .SetStrokeWidth(renderer_.render_settings.underlayer_width)
            .SetStrokeLineCap(StrokeLineCap::ROUND)
            .SetStrokeLineJoin(StrokeLineJoin::ROUND));

        map.Add(Text()
            .SetPosition(projector(stop.coordinates))
            .SetOffset(renderer_.render_settings.stop_label_offset)
            .SetFontSize(renderer_.render_settings.stop_label_font_size)
            .SetFontFamily("Verdana")
            .SetData(stop.name)
            .SetFillColor("black"));
    }

    return map;
}