#include <string>
#include <iostream>
#include <sstream>

#include "transport_catalogue.h"

namespace transport {

	void PrintRoute(ostream& out, const TransportCatalogue& transport_catalogue, const std::string& route_name) {
		try {
			const auto& r = transport_catalogue.FindRoute(route_name);
			out << "Bus " << r.name << ": "
				<< std::fixed << std::setprecision(5) << r.GetStopsOnRoute() << " stops on route, "
				<< r.CountUniqueStops() << " unique stops, "
				<< transport_catalogue.ComputeRouteLength(r) << " route length, "
				<< transport_catalogue.ComputeCurvature(r) << " curvature" << std::endl;
		}
		catch (std::out_of_range& e) {
			out << "Bus " << route_name << ": " << "not found"s << std::endl;
		}
	}

	void PrintStop(ostream& out, const TransportCatalogue& transport_catalogue, const std::string& stop_name) {
		std::stringstream result;
		result << "Stop " << stop_name << ": ";
		try {
			const auto& buses = transport_catalogue.GetBusList(stop_name);
			if (buses.empty()) {
				result << "no buses";
			}
			else {
				result << "buses ";
				for (const auto& bus_name : buses) {
					result << bus_name << " ";
				}
			}
		}
		catch (std::out_of_range& e) {
			result << "not found";
		}
		out << result.str() << std::endl;
	}

	void ProcessRequest(istream& input, ostream& out, const TransportCatalogue& transport_catalogue) {
		int N;
		input >> N;
		std::string request;
		std::getline(input, request);

		for (int i = 0; i < N; i++) {
			std::getline(input, request);
			std::string command = request.substr(0, request.find(' '));
			if (command == "Bus") {
				const std::string& route_name = request.substr(request.find(' ') + 1);
				PrintRoute(out, transport_catalogue, route_name);
			}
			else if (command == "Stop") {
				const std::string& stop_name = request.substr(request.find(' ') + 1);
				PrintStop(out, transport_catalogue, stop_name);
			}
		}
	}
}