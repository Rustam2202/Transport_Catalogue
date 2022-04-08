#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
//#include "transport_catalogue.h"

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

	fstream file("s10_final_opentest_1.json"s); // s10_final_opentest_1
	istream& strm(file);
	ReadJSON(strm, std::cout);


	//TransportCatalogue catalogue;

	
	//fstream file("s10_final_opentest_1.json"s); // s10_final_opentest_1
	//istream& strm(file);
	//
	//ReadJSON(strm, std::cout);

	//ReadJSON(catalogue, std::cin, std::cout);
}