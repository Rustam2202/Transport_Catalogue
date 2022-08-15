#include <transport_catalogue.pb.h>

#include "json_reader.h"

#include <fstream>
#include <iostream>
#include <ostream>

using namespace std;

void Serialization(std::istream& strm = cin);

void DeSerialization(std::istream& strm = std::cin, std::ostream& output = std::cout);