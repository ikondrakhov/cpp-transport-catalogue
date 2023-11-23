#include <vector>
#include <deque>
#include <string>
#include <iostream>

#include "transport_catalogue.h"

namespace transport {

	using namespace std;

	std::string_view Trim(std::string_view sv) {
		sv.remove_prefix(sv.find_first_not_of(' '));
		sv.remove_suffix(sv.size() - sv.find_last_not_of(' ') - 1);
		return sv;
	}

	std::deque<std::string_view> SplitAndTrim(std::string_view route, std::string delimeter) {
		std::deque<std::string_view> stops;
		for (size_t pos = 0; (pos = route.find(delimeter)) != std::string::npos; route.remove_prefix(pos + delimeter.size())) {
			stops.push_back(Trim(route.substr(0, pos)));
		}
		stops.push_back(Trim(route));
		return stops;
	}

	void AddStops(TransportCatalogue& transport_catalogue, const std::vector<std::string>& add_stop_commands) {
		for (std::string_view command : add_stop_commands) {
			std::string_view stop_name = SplitAndTrim(command, ":")[0];

			std::deque<std::string_view> splited_by_comma = SplitAndTrim(SplitAndTrim(command, ":")[1], ",");
			std::string_view first_coordintate = splited_by_comma[0];
			splited_by_comma.pop_front();
			std::string_view second_coordinate = splited_by_comma[0];
			splited_by_comma.pop_front();

			std::map<std::string, int> stop_to_distance;
			for (std::string_view distance_to_stop : splited_by_comma) {
				std::string_view distance = SplitAndTrim(distance_to_stop, "m ")[0];
				std::string_view stop_name = SplitAndTrim(distance_to_stop, " to ")[1];
				stop_to_distance[std::string(stop_name)] = std::atoi(distance.data());
			}
			transport_catalogue.AddStop({ std::string(stop_name), {std::atof(first_coordintate.data()), std::atof(second_coordinate.data())}, stop_to_distance });
		}
	}

	void AddBuses(TransportCatalogue& transport_catalogue, const std::vector<std::string>& add_route_commands) {
		for (std::string_view command : add_route_commands) {
			std::string_view route_name = command.substr(0, command.find(':'));
			if (command.find('-') != std::string::npos) {
				std::deque<std::string_view> route_one_way = SplitAndTrim(command.substr(command.find(':') + 1), "-");
				std::deque<std::string_view> route = route_one_way;
				std::reverse(route_one_way.begin(), route_one_way.end());
				route_one_way.pop_front();
				for (std::string_view stop : route_one_way) {
					route.push_back(stop);
				}
				transport_catalogue.AddRoute({ std::string(route_name), route });
			}
			else if (command.find('>') != std::string::npos) {
				transport_catalogue.AddRoute({ std::string(route_name), SplitAndTrim(command.substr(command.find(':') + 1), ">") });
			}
		}
	}

	void ReadCatalogue(std::istream& input, TransportCatalogue& transport_catalogue) {
		int N;
		input >> N;
		std::string line;
		std::getline(input, line);
		std::vector<std::string> add_stop_commands;
		std::vector<std::string> add_route_commands;
		for (int i = 0; i < N; i++) {
			std::getline(input, line);
			std::string command = line.substr(0, line.find(' '));
			line.erase(0, line.find(' '));
			line.erase(0, line.find_first_not_of(' '));
			if (command == "Stop"s) {
				add_stop_commands.push_back(line);
			}
			else if (command == "Bus"s) {
				add_route_commands.push_back(line);
			}
		}

		AddStops(transport_catalogue, add_stop_commands);
		AddBuses(transport_catalogue, add_route_commands);
	}
}