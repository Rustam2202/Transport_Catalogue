#pragma once

#include "input_reader.h"
#include "json_reader.h"
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
    std::fstream file("input.json"s);
    ReadJSON(file);
  // ReadJSON("input.json"s);


    //{
    //    using namespace transport_catalogue;
    //    TransportCatalogue transport_catalogue;
    //    InputReader(transport_catalogue);
    //    OutputReader(transport_catalogue);
    //}
}