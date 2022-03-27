#pragma once

#include "input_reader.h"
#include "json_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

//#include <iostream>
#include <string>

int main() {
  
    /*
    * Примерная структура программы:
    *
    * Считать JSON из stdin
    * Построить на его основе JSON базу данных транспортного справочника
    * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
    * с ответами Вывести в stdout ответы в виде JSON
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