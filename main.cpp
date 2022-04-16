#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <cassert>
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

	fstream input_file("input_svg.json"s); // s10_final_opentest_1
	fstream output_file;
	istream& strm(input_file);
	output_file.open("output_svg.json");
	//ReadJSON(std::cin, std::cout); 
	ReadJSON(strm, output_file);
	output_file.close();
}