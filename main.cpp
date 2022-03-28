#pragma once

#include "input_reader.h"
#include "json_reader.h"
#include "request_handler.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

//#include <iostream>
#include <string>

int main() {

	/*
	* ��������� ��������� ���������:
	*
	* ������� JSON �� stdin
	* ��������� �� ��� ������ JSON ���� ������ ������������� �����������
	* ��������� ������� � �����������, ����������� � ������� "stat_requests", �������� JSON-������
	* � �������� ������� � stdout ������ � ���� JSON
	*/
	using namespace std::literals;

	TransportCatalogue catalogue;
	std::fstream file("input.json"s);
	catalogue = ReadJSON(file);

	PrintJSON(catalogue);
}