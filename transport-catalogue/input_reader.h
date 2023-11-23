#pragma once

#include <vector>
#include <deque>
#include <string>
#include <iostream>

namespace transport {

	using namespace std;

	std::string_view Trim(std::string_view sv);
	std::deque<std::string_view> SplitAndTrim(std::string_view route, char delimeter);
	void AddStops(TransportCatalogue& transport_catalogue, const std::vector<std::string>& add_stop_commands);
	void AddBuses(TransportCatalogue& transport_catalogue, const std::vector<std::string>& add_route_commands);
	void ReadCatalogue(std::istream& input, TransportCatalogue& transport_catalogue);

}