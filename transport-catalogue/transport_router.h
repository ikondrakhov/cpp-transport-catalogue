#include <map>
#include <string>
#include <set>
#include <queue>
#include <memory>

#include "graph.h"
#include "domain.h"
#include "router.h"

namespace graph {
	class TransportRouter {
	public:
		TransportRouter() = default;
		void InitRouter(const std::map<std::string, transport::Stop>& name_to_stop, const std::set<transport::Route>& routes, transport::RoutingSettings routing_settings);
		std::queue<transport::RouteItem> FindShortestRoute(const std::string_view from, const std::string_view to) const;

	private:
		std::unordered_map<std::string_view, size_t> stop_ids;
		DirectedWeightedGraph<transport::RouteItem> graph_;
		transport::RoutingSettings routing_settings_;
		std::unique_ptr<Router<transport::RouteItem>> router;
	};
}