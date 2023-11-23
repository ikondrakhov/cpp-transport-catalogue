#pragma once

#include <string>
#include <iostream>

#include "transport_catalogue.h"

namespace transport {

    void PrintRoute(const TransportCatalogue& transport_catalogue, const TransportCatalogue::Route& r);
    void PrintStop(const TransportCatalogue& transport_catalogue, const std::string& stop_name);
    void ProcessRequest(const TransportCatalogue& transport_catalogue);
}