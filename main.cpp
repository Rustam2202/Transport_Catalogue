#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
//#include "transport_catalogue.h"

#include <fstream>
#include <string>

int main() {

	/*
	* ��������� ��������� ���������:
	*
	* ������� JSON �� stdin
	* ��������� �� ��� ������ JSON ���� ������ ������������� �����������
	* ��������� ������� � �����������, ����������� � ������� "stat_requests", �������� JSON-������
	* � ��������. ������� � stdout ������ � ���� JSON
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