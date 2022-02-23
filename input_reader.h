// чтение запросов на заполнение базы

#pragma once

#include "transport_catalogue.h"

#include <algorithm>
#include <iostream>
#include <string_view>

using namespace std;

Stop ReadInputStop(istream& query);

Bus ReadInputBus(TransportCatalogue& trans_cat, string_view str);

void InputReader(TransportCatalogue& trans_cat);