#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

using namespace json;
using namespace renderer;
using namespace transport_catalogue;

class RequestHandler : public MapRenderer, TransportCatalogue {
public:

	RequestHandler(TransportCatalogue& catalogue, MapRenderer& renderer) :catalogue_(catalogue), renderer_(renderer) {}

	Dict MakeDictStop(int request_id, std::string_view stop_name);
	Dict MakeDictBus(int request_id, std::string_view bus_name);
	Dict MakeDictMap(int request_id);

	void SetZoom();
	void AddBusesData();
	void DrawMap();

private:
	transport_catalogue::TransportCatalogue& catalogue_;
	renderer::MapRenderer& renderer_;
};
