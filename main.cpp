#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

#include <iostream>
#include <string>

int main() {

    stdin;

    /*
    * Примерная структура программы:
    *
    * Считать JSON из stdin
    * Построить на его основе JSON базу данных транспортного справочника
    * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
    * с ответами Вывести в stdout ответы в виде JSON
    */

	using namespace transport_catalogue;
	TransportCatalogue transport_catalogue;
	InputReader(transport_catalogue);
	OutputReader(transport_catalogue);
}