#pragma once

#include <iostream>
#include <deque>

#include "json.h"
#include "transport_catalogue.h"
#include "svg.h"

svg::Color GetColor(json::Node color);
void ProcessRequests(std::istream& input, std::ostream& output, transport::TransportCatalogue& catalogue);
std::string ColorToString(json::Node color);