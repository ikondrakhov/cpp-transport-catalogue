#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

#include <iostream>
#include <sstream>
#include <cassert>

int main() {
    transport::TransportCatalogue transport_catalogue;
    transport::ReadCatalogue(std::cin, transport_catalogue);
    transport::ProcessRequest(std::cin, std::cout, transport_catalogue);
    return 0;
}