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

class RequestHandler {
public:

	RequestHandler(TransportCatalogue& db, const renderer::MapRenderer& renderer) :db_(db), renderer_(renderer) {	}

	void InsertStops(Array base);
	void InsertStopsDistances(Array base);
	void InsertBuses(Array base);
	void CompileStats(Array base, Array& stats);

	// Этот метод будет нужен в следующей части итогового проекта
	//svg::Document RenderMap() const;

private:
	Dict MakeDictStop(int request_id, std::string_view stop_name);
	Dict MakeDictBus(int request_id, std::string_view bus_name);

	transport_catalogue::TransportCatalogue& db_;
	const renderer::MapRenderer& renderer_;
};
