#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

using namespace json;
using namespace renderer;
using namespace transport_catalogue;

using PointInfo = std::vector<std::pair<std::string_view, geo::Coordinates>>;

class RequestHandler : public MapRenderer, TransportCatalogue {
public:

	RequestHandler(TransportCatalogue& catalogue, MapRenderer& renderer) :catalogue_(catalogue), renderer_(renderer) {}

	void InsertStops(Array base);
	void InsertStopsDistances(Array base);
	void InsertBuses(Array base);
	void CompileStats(Array base, Array& stats);
	void SetZoom();
	void AddBusesData();

	void DrawMap() {
		renderer_.AddBusesLines();
		renderer_.Sorting();
		renderer_.AddBusesNames();
		renderer_.AddCircle();
		renderer_.AddStopsNames();
	}

private:
	Dict MakeDictStop(int request_id, std::string_view stop_name);
	Dict MakeDictBus(int request_id, std::string_view bus_name);
	Dict MakeDictMap(int request_id);

	transport_catalogue::TransportCatalogue& catalogue_;
	renderer::MapRenderer& renderer_;
};
