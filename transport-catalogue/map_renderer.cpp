#include "map_renderer.h"

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

svg::Document renderer::MapRenderer::RenderMap(const std::set<transport::Route>& routes, 
                        const std::map<std::string_view, Coordinates>& stop_coordinates) const {
    using namespace svg;

    std::vector<Coordinates> coordinates;
    std::set<std::string_view> stops;
    // get stops whitch are used in routes
    for (const auto& route : routes) {
        for (const auto& stop_name : route.stops) {
            coordinates.push_back(stop_coordinates.at(stop_name));
            stops.insert(stop_name);
        }
    }

    SphereProjector projector(coordinates.begin(), coordinates.end(), render_settings.width, render_settings.height, render_settings.padding);

    std::vector<svg::Color> colors = render_settings.color_palette;

    Document map;

    // Add routes to map
    int color_num = 0;
    for (const auto& route : routes) {
        if (route.stops.size() > 1) {
            Polyline route_line = Polyline()
                .SetFillColor("none")
                .SetStrokeColor(colors[color_num])
                .SetStrokeWidth(render_settings.line_width)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND);
            for (const auto& stop_name : route.stops) {
                Point p = projector(stop_coordinates.at(stop_name));
                route_line.AddPoint(p);
            }
            map.Add(route_line);
            color_num = (color_num + 1) % colors.size();
        }
    }

    // Add route names
    color_num = 0;
    for (const auto& route : routes) {
        if (route.stops.size() > 1) {
            Text route_name = Text()
                .SetFillColor(colors[color_num])
                .SetPosition(projector(stop_coordinates.at(route.stops[0])))
                .SetOffset(render_settings.bus_label_offset)
                .SetFontSize(render_settings.bus_label_font_size)
                .SetFontFamily("Verdana")
                .SetFontWeight("bold")
                .SetData(route.name);
            Text under_route_name = Text()
                .SetFillColor(render_settings.underlayer_color)
                .SetStrokeColor(render_settings.underlayer_color)
                .SetStrokeWidth(render_settings.underlayer_width)
                .SetPosition(projector(stop_coordinates.at(route.stops[0])))
                .SetOffset(render_settings.bus_label_offset)
                .SetFontSize(render_settings.bus_label_font_size)
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
                    .SetPosition(projector(stop_coordinates.at(route.stops[route.stops.size() / 2])))
                    .SetOffset(render_settings.bus_label_offset)
                    .SetFontSize(render_settings.bus_label_font_size)
                    .SetFontFamily("Verdana")
                    .SetFontWeight("bold")
                    .SetData(route.name);
                under_route_name = Text()
                    .SetFillColor(render_settings.underlayer_color)
                    .SetStrokeColor(render_settings.underlayer_color)
                    .SetStrokeWidth(render_settings.underlayer_width)
                    .SetPosition(projector(stop_coordinates.at(route.stops[route.stops.size() / 2])))
                    .SetOffset(render_settings.bus_label_offset)
                    .SetFontSize(render_settings.bus_label_font_size)
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
        map.Add(Circle()
            .SetCenter(projector(stop_coordinates.at(stop_name)))
            .SetRadius(render_settings.stop_radius)
            .SetFillColor("white"));
    }

    // Add stop names
    for (std::string_view stop_name : stops) {
        map.Add(Text()
            .SetPosition(projector(stop_coordinates.at(stop_name)))
            .SetOffset(render_settings.stop_label_offset)
            .SetFontSize(render_settings.stop_label_font_size)
            .SetFontFamily("Verdana")
            .SetData(stop_name.data())
            .SetFillColor(render_settings.underlayer_color)
            .SetStrokeColor(render_settings.underlayer_color)
            .SetStrokeWidth(render_settings.underlayer_width)
            .SetStrokeLineCap(StrokeLineCap::ROUND)
            .SetStrokeLineJoin(StrokeLineJoin::ROUND));

        map.Add(Text()
            .SetPosition(projector(stop_coordinates.at(stop_name)))
            .SetOffset(render_settings.stop_label_offset)
            .SetFontSize(render_settings.stop_label_font_size)
            .SetFontFamily("Verdana")
            .SetData(stop_name.data())
            .SetFillColor("black"));
    }

    return map;
}