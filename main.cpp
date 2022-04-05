#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"

#include <fstream>
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
	
	fstream file("speed_input.json"s);
	istream& strm(file);
	
	ReadJSON(catalogue,strm, std::cout);

	//ReadJSON(catalogue, std::cin, std::cout);
}