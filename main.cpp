#pragma once

#include "input_reader.h"
#include "json_reader.h"
#include "request_handler.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

//include <iostream>
#include <string>

int main() {

	/*
	* Примерная структура программы:
	*
	* Считать JSON из stdin
	* Построить на его основе JSON базу данных транспортного справочника
	* Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
	* с ответами. Вывести в stdout ответы в виде JSON
	*/
	using namespace std;

	TransportCatalogue catalogue;

	
	fstream file("input.json"s);
	istream& istrm(file);
	ReadJSON(istrm, catalogue);

	PrintJSON(catalogue);
}