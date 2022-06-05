#pragma once

#include "json.h"
#include "json_builder.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

using namespace json;
using namespace renderer;
using namespace transport_catalogue;

class RequestHandler : public MapRenderer, TransportCatalogue {
public:
	RequestHandler(TransportCatalogue& catalogue, MapRenderer& renderer) :catalogue_(catalogue), renderer_(renderer) {}
	Node MakeDictBus(int request_id, std::string_view bus_name);
	Node MakeDictMap(int request_id);
	Node MakeDictStop(int request_id, std::string_view stop_name);

	void SetZoom();
	void AddBusesData();
	void DrawMap();

private:
	transport_catalogue::TransportCatalogue& catalogue_;
	renderer::MapRenderer& renderer_;
};
