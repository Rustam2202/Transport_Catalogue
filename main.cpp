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

	fstream input_file("input_svg.json"s); // s10_final_opentest_1
	fstream output_file;
	istream& strm(input_file);
	output_file.open("output_svg.svg");
	ReadJSON(strm, output_file /*std::cout*/);
	output_file.close();


	//TransportCatalogue catalogue;

	
	//fstream file("s10_final_opentest_1.json"s); // s10_final_opentest_1
	//istream& strm(file);
	//
	//ReadJSON(strm, std::cout);

	//ReadJSON(catalogue, std::cin, std::cout);
}

#include <string>
#include <fstream>
#include <iostream>

//int main()
//{
//	std::string filename = "example.123";
//
//	std::fstream fs;
//
//	fs.open(filename);
//
//	if (!fs.is_open())
//	{
//		fs.clear();
//		fs.open(filename, std::ios::out); //Create file.
//		fs.close();
//		fs.open(filename);
//	}
//
//	std::cout << std::boolalpha;
//	std::cout << "fs.is_open() = " << fs.is_open() << '\n';
//	std::cout << "fs.good() = " << fs.good() << '\n';
//}