#pragma once

#include <string>
#include <iostream>

#include "transport_catalogue.h"

namespace transport {

    void PrintRoute(ostream& out, const TransportCatalogue& transport_catalogue, const TransportCatalogue::Route& r);
    void PrintStop(ostream& out, const TransportCatalogue& transport_catalogue, const std::string& stop_name);
    void ProcessRequest(istream& input, ostream& out, const TransportCatalogue& transport_catalogue);
}