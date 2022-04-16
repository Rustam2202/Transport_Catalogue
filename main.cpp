#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <cassert>
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

	fstream input_file("input_svg.json"s); // s10_final_opentest_1
	fstream output_file;
	istream& strm(input_file);
	output_file.open("output_svg.json");
	//ReadJSON(std::cin, std::cout); 
	ReadJSON(strm, output_file);
	output_file.close();
}