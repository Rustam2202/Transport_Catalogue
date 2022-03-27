#pragma once

struct Coordinates {
	double lat;
	double lng;
};

namespace geo {
	double ComputeDistance(Coordinates from, Coordinates to);
}