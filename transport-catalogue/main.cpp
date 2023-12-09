#include "transport_catalogue.h"
#include "json_reader.h"

#include <iostream>
#include <sstream>
#include <cassert>

int main() {
    transport::TransportCatalogue transport_catalogue;
    ProcessRequests(std::cin, std::cout, transport_catalogue);
    return 0;
}