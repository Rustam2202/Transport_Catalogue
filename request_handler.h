#pragma once

#include "graph.h"
#include "json.h"
#include "json_builder.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

using namespace json;
using namespace renderer;
using namespace transport_catalogue;

class RequestHandler : public MapRenderer, TransportCatalogue {
public:
	RequestHandler(TransportCatalogue& catalogue, MapRenderer& renderer, TransportRoter& router) :
		catalogue_(catalogue),
		renderer_(renderer),
		router_(router)
	{}

	Node MakeDictBus(int request_id, std::string_view bus_name);
	Node MakeDictMap(int request_id);
	Node MakeDictStop(int request_id, std::string_view stop_name);
	Node MakeDictRoute(int request_id, std::string_view from, std::string_view to);

	void SetZoom();
	void AddBusesData();
	void DrawMap();

	TransportCatalogue& GetCatalogue() {
		return catalogue_;
	}

private:
	TransportCatalogue& catalogue_;
	MapRenderer& renderer_;
	TransportRoter& router_;
};
