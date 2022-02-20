// чтение запросов на вывод и сам вывод

#pragma once

#include "transport_catalogue.h"

#include <iostream>

using namespace std;

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    if (r.buses.size() == 0)
        os << "No bus";
    else {
        for (const auto bus : r.buses) {
            if (bus == r.buses[0])
                os << bus;
            else
                os << " " << bus;
        }
    }
    return os;
}

void OutputReader(TransportCatalogue& trans_cat) {
    int query_count;

    cout << "Enter count of count to show buses: ";
    cin >> query_count;

    int query_numb = 0;
   
    while (query_numb < query_count) {
        int bus_number;
      //  cout << "Enter Bus number: ";
        cin >> bus_number;

    }
}

/*
10
Stop Tolstopaltsevo : 55.611087, 37.208290
Stop Marushkino : 55.595884, 37.209755
Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka : 55.632761, 37.333324
Stop Biryulyovo Zapadnoye : 55.574371, 37.651700
Stop Biryusinka : 55.581065, 37.648390
Stop Universam : 55.587655, 37.645687
Stop Biryulyovo Tovarnaya : 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164

Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750 : 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751 : not found
*/