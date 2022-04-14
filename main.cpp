#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
//#include "transport_catalogue.h"

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
	{
		fstream input_file("s10_final_opentest_3.json"s); // s10_final_opentest_1
		fstream output_file;
		istream& strm(input_file);
		output_file.open("output_svg.json");
		//ReadJSON(strm, /*output_file*/ std::cout);
		ReadJSON(strm, output_file);
		output_file.close();
	}

	//{
	//	fstream input_test("s10_final_opentest_3.json"s); // s10_final_opentest_1 // json
	//	fstream input_answer("s10_final_opentest_1_answer.json");
	//	fstream output_test, output_answer;
	//	istream& istrm_test(input_test);
	//	output_test.open("test_1.svg"); // test_1 // output_svg
	//	ReadJSON(istrm_test, output_test);
	//	output_test.close();
	//}
}