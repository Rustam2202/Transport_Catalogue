#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"

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

	TransportCatalogue catalogue;
	
	fstream file("speed_input.json"s);
	istream& strm(file);
	
	ReadJSON(catalogue,strm, std::cout);

	//ReadJSON(catalogue, std::cin, std::cout);
}