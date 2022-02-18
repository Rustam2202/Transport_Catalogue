#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

struct Buses {
    string bus;
    vector<string> stops;
};

struct Stops
{
    string stop;
    vector<string> buses;
};

istream& operator>>(istream& is, Query& q) {
    string str;
    is >> str;
    if (str == "NEW_BUS") {
        q.type = QueryType::NewBus;
        is >> q.bus;
        int stop_count;
        is >> stop_count;
        q.stops.resize(stop_count);
        for (int i = 0; i < stop_count; ++i)
            is >> q.stops[i];
    }
    else if (str == "BUSES_FOR_STOP") {
        q.type = QueryType::BusesForStop;
        is >> q.stop;
    }
    else if (str == "STOPS_FOR_BUS") {
        q.type = QueryType::StopsForBus;
        is >> q.bus;
    }
    else if (str == "ALL_BUSES")
        q.type = QueryType::AllBuses;
    return is;
}

struct BusesForStopResponse {
    // Наполните полями эту структуру
    //map<string, vector<string>> stops_for_buses;
    //string stop;
    vector<string>buses;
};

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

struct StopsForBusResponse {
    vector<Stops> stops_vec;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    if (r.stops_vec.size() == 0)
        os << "No bus";
    else {
        int i = 0;
        for (auto stop : r.stops_vec) {
            os << "Stop " << stop.stop << ": ";
            if (stop.buses.size() == 0)
                cout << "no interchange";
            else {
                for (string bus : stop.buses) {
                    if (bus == stop.buses[0])
                        os << bus;
                    else
                        os << " " << bus;
                }
            }
            i++;
            if (i != r.stops_vec.size())
                cout << endl;
        }
    }
    return os;
}

struct AllBusesResponse {
    map<string, vector<string>> buses_for_stops;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    if (r.buses_for_stops.size() == 0)
        os << "No buses";
    else {
        int i = 0;
        for (const auto bus : r.buses_for_stops) {
            os << "Bus " << bus.first << ": ";
            for (auto stop : bus.second) {
                if (stop == bus.second[0])
                    os << stop;
                else
                    os << " " << stop;
            }
            i++;
            if (i != r.buses_for_stops.size())
                cout << endl;
        }
    }
    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        // Реализуйте этот метод
        buses_.push_back({ bus,stops });
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        // Реализуйте этот метод
        BusesForStopResponse buses_and_stop;
        //buses_and_stop.stop = stop;
        for (auto bus : buses_) {
            if (count(bus.stops.begin(), bus.stops.end(), stop)) {
                buses_and_stop.buses.push_back(bus.bus);
            }
        }
        return buses_and_stop;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        // Реализуйте этот метод
        StopsForBusResponse stops_and_bus;
        vector<string> stops_temp;


        for (auto bus_it : buses_) {
            if (bus_it.bus == bus) {
                stops_temp = bus_it.stops;
                break;
            }
        }

        for (string stop : stops_temp) {
            vector<string> buses;
            for (auto bus_in_stops : buses_) {
                if (count(bus_in_stops.stops.begin(), bus_in_stops.stops.end(), stop)) {
                    if (bus_in_stops.bus != bus)
                        buses.push_back(bus_in_stops.bus);
                }
            }
            stops_and_bus.stops_vec.push_back({ stop,buses });
        }

        return stops_and_bus;
    }

    AllBusesResponse GetAllBuses() const {
        // Реализуйте этот метод
        AllBusesResponse result;
        for (auto bus : buses_) {
            result.buses_for_stops[bus.bus] = bus.stops;
        }
        return result;
    }

private:
    vector<Buses> buses_;
};

void QueryCalc() {
    int query_count = 10;
    Query q;
    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
        case QueryType::NewBus:
            bm.AddBus(q.bus, q.stops);
            break;
        case QueryType::BusesForStop:
            cout << bm.GetBusesForStop(q.stop) << endl;
            break;
        case QueryType::StopsForBus:
            cout << bm.GetStopsForBus(q.bus) << endl;
            break;
        case QueryType::AllBuses:
            cout << bm.GetAllBuses() << endl;
            break;
        }
    }
}

void TestInOut() {
    istringstream input;
    ostringstream output;
    Query q;
    BusManager bm;

    input.str("NEW_BUS GoldenRing 4 Vladimir Rostov Ivanovo Posad");
    input >> q;
    bm.AddBus(q.bus, q.stops);
    //cout << bm.GetAllBuses()<<endl;
    output << bm.GetAllBuses();
    assert(output.str() == "Bus GoldenRing: Vladimir Rostov Ivanovo Posad");
    input.str("STOPS_FOR_BUS GoldenRing");
    input >> q;


    /*
    input.str("ALL_BUSES\nBUSES_FOR_STOP Marushkino\nSTOPS_FOR_BUS 32K");

    assert(output.str() == "No buses\nNo stop\nNo bus");
    input.clear();
    output.clear();
    input.str("NEW_BUS 32 3 Tolstopaltsevo Marushkino Vnukovo\nNEW_BUS 32K 6 Tolstopaltsevo Marushkino Vnukovo Peredelkino Solntsevo Skolkovo\nBUSES_FOR_STOP Vnukovo");
    assert(output.str() == "32 32K");
    input.clear();
    output.clear();
    input.str("NEW_BUS 950 6 Kokoshkino Marushkino Vnukovo Peredelkino Solntsevo Troparyovo\nNEW_BUS 272 4 Vnukovo Moskovsky Rumyantsevo Troparyovo\nSTOPS_FOR_BUS 272");
    assert(output.str()=="Stop Vnukovo: 32 32K 950\nStop Moskovsky : no interchange\nStop Rumyantsevo : no interchange\nStop Troparyovo : 950");
    input.clear();
    output.clear();
    input.str("ALL_BUSES ");
    assert(output.str() == "Bus 272: Vnukovo Moskovsky Rumyantsevo Troparyovo\nBus 32: Tolstopaltsevo Marushkino Vnukovo\nBus 32K : Tolstopaltsevo Marushkino Vnukovo Peredelkino Solntsevo Skolkovo\nBus 950 : Kokoshkino Marushkino Vnukovo Peredelkino Solntsevo Troparyovo");
    */
}

void TestInput() {
    istringstream input;

    input.str("NEW_BUS golden_ring 4 Vladimir Rostov Ivanovo Posad");
    Query q;
    input >> q;
    BusManager bm;
    assert(q.type == QueryType::NewBus);
    assert(q.bus == "golden_ring");
    assert(q.stops[0] == "Vladimir");
    assert(q.stops[1] == "Rostov");
    assert(q.stops[2] == "Ivanovo");
    assert(q.stops[3] == "Posad");
    input.clear();

    input.str("BUSES_FOR_STOP Rostov");
    Query s;
    input >> s;
    assert(s.type == QueryType::BusesForStop);
    assert(s.stop == "Rostov");
    input.clear();

    input.str("STOPS_FOR_BUS golden_ring");
    Query b;
    input >> b;
    assert(b.type == QueryType::StopsForBus);
    assert(b.bus == "golden_ring");
    input.clear();
}

void TestOutput() {
    AllBusesResponse all;
    ostringstream output;


    /*
    all.buses_for_stops["golden_ring"] = { "Vladimir", "Rostov", "Ivanovo", "Posad" };
    output << all;
    cout << all;
    assert(output.str() == "Bus golden_ring Vladimir Rostov Ivanovo Posad");
    */
}



int main()
{
    int query_count = 10;
    Query q;
    BusManager bm;

    TestInOut();
    //TestInput();
    //TestOutput();

    return 0;

    //cin >> query_count;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
        case QueryType::NewBus:
            bm.AddBus(q.bus, q.stops);
            break;
        case QueryType::BusesForStop:
            cout << bm.GetBusesForStop(q.stop) << endl;
            break;
        case QueryType::StopsForBus:
            cout << bm.GetStopsForBus(q.bus) << endl;
            break;
        case QueryType::AllBuses:
            cout << bm.GetAllBuses() << endl;
            break;
        }
    }

}

/*
ALL_BUSES
BUSES_FOR_STOP Marushkino
STOPS_FOR_BUS 32K
NEW_BUS 32 3 Tolstopaltsevo Marushkino Vnukovo
NEW_BUS 32K 6 Tolstopaltsevo Marushkino Vnukovo Peredelkino Solntsevo Skolkovo
BUSES_FOR_STOP Vnukovo
NEW_BUS 950 6 Kokoshkino Marushkino Vnukovo Peredelkino Solntsevo Troparyovo
NEW_BUS 272 4 Vnukovo Moskovsky Rumyantsevo Troparyovo
STOPS_FOR_BUS 272
ALL_BUSES
*/