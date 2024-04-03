#pragma once

#include <optional>
#include <unordered_set>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_reader.h"
 
 class RequestHandler {
 public:
     RequestHandler(const transport::TransportCatalogue& db, const renderer::MapRenderer& renderer);

     // Возвращает информацию о маршруте (запрос Bus)
     json::Dict GetBusStat(const std::string_view& bus_name) const;
     
     json::Dict GetBusesByStop(const std::string_view& stop_name) const;

     json::Dict FindShortestRoute(const std::string_view from, const std::string_view to) const;

 private:
     const transport::TransportCatalogue& db_;
     const renderer::MapRenderer& renderer_;
 };
