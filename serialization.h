//#include <transport_catalogue.pb.h>

#include "json_reader.h"

#include <fstream>
#include <iostream>

using namespace std;

void Serialization(std::istream& strm = cin) {
	TransportCatalogue catalogue = MakeBase(strm);
}