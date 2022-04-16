#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

#include <algorithm>
#include <execution>
/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)

using namespace json;
using namespace transport_catalogue;
using namespace std::literals;

using PointInfo = std::vector<std::pair<std::string_view, geo::Coordinates>>;

class RequestHandler : public renderer::MapRenderer, transport_catalogue::TransportCatalogue {
public:

	RequestHandler(TransportCatalogue& catalogue, renderer::MapRenderer& renderer) :catalogue_(catalogue), renderer_(renderer) {
		//renderer_.MakeSphereProjector(FindAllCoordinaties());
	}

	void InsertStops(Array base);
	void InsertStopsDistances(Array base);
	void InsertBuses(Array base);
	void CompileStats(Array base, Array& stats);

	//svg::Document RenderMap() const {
	//	//return renderer_.Rendering();
	//}

	void SetZoom() {
		std::vector<geo::Coordinates> result;
		for (const auto& stop : catalogue_.GetStopInfo()) {
			if (!stop.second.empty()) {
				result.push_back(stop.first->coodinates);
			}
		}
		renderer_.MakeSphereProjector(result);
	}

	void AddBusesData() {
		for (const Bus& bus : catalogue_.GetBuses()) {
			if (bus.stops_vector.empty()) {
				continue;
			}
			for (const Stop* stop : bus.stops_vector) {
				renderer_.AddBusWithStops(bus.bus_name, stop->stop_name, stop->coodinates);
				renderer_.AddPoint(stop->stop_name, stop->coodinates);
			}
			if (bus.is_ring == false) {
				for (int i = bus.stops_vector.size() - 2; i >= 0; --i) {
					//polyline.AddPoint(bus.second[i].coordinates);
					renderer_.AddBusWithStops(bus.bus_name, bus.stops_vector[i]->stop_name, bus.stops_vector[i]->coodinates);	
				}
			}
		}
	}

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
