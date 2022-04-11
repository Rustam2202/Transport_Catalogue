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

class RequestHandler : public renderer::MapRenderer, transport_catalogue::TransportCatalogue {
public:

	RequestHandler(TransportCatalogue& catalogue, renderer::MapRenderer& renderer) :catalogue_(catalogue), renderer_(renderer) {
		//renderer_.MakeSphereProjector(FindAllCoordinaties());
	}

	void InsertStops(Array base);
	void InsertStopsDistances(Array base);
	void InsertBuses(Array base);
	void CompileStats(Array base, Array& stats);

	// Этот метод будет нужен в следующей части итогового проекта
	svg::Document RenderMap() const {
		//return renderer_.Rendering();
	}

	std::vector<geo::Coordinates> FindAllCoordinaties() {
		std::vector<geo::Coordinates> result;
		for (auto stop : catalogue_.GetStops()) {
			result.push_back(stop.coodinates);
		}
		return result;
	}

	void CalculateSphereProjector() {
		renderer_.MakeSphereProjector(FindAllCoordinaties());
	}

	void AddBusesLines() {
		for (auto bus : catalogue_.GetBuses()) {
			std::vector<geo::Coordinates> result;
			for (Stop* stop : bus.stops_vector) {
				result.push_back(stop->coodinates);
			}
			renderer_.AddPolyline(result);
		}
	}

	void AddBusText() {
		/*std::deque<Bus> buses_vect(catalogue_.GetBuses());
			std::sort(buses_vect.begin(), buses_vect.end(),
			[](const Bus& lhs, const Bus& rhs) {return lhs.bus_name < rhs.bus_name; });*/

		for (auto bus : catalogue_.GetBuses()) {
			if (bus.is_ring) {

			}
		}
	}

	void AddRounds() {

	}

	void AddStopText() {

	}


private:
	Dict MakeDictStop(int request_id, std::string_view stop_name);
	Dict MakeDictBus(int request_id, std::string_view bus_name);

	transport_catalogue::TransportCatalogue& catalogue_;
	renderer::MapRenderer& renderer_;
};
