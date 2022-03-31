#pragma once

#include "input_reader.h"
#include "json_reader.h"
#include "request_handler.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

//include <iostream>
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
	
	fstream file("input2.json"s);
	istream& strm(file);
	
	ReadJSON(catalogue,strm, std::cout);
	//ReadJSON(strm,std::cout);
}