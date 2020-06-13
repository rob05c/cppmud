#include "dir.hpp"
#include <string>

Direction reverse_direction(Direction d) {
	switch(d) {
	case DirectionNorth:
		return DirectionSouth;
	case DirectionEast:
		return DirectionWest;
	case DirectionSouth:
		return DirectionNorth;
	case DirectionWest:
		return DirectionEast;
	case DirectionNorthEast:
		return DirectionSouthWest;
	case DirectionNorthWest:
		return DirectionSouthEast;
	case DirectionSouthEast:
		return DirectionNorthWest;
	case DirectionSouthWest:
		return DirectionNorthEast;
	default:
		return DirectionInvalid;
	};
}

std::string direction_str(Direction d) {
	switch(d) {
	case DirectionNorth:
		return "north";
	case DirectionEast:
		return "east";
	case DirectionSouth:
		return "south";
	case DirectionWest:
		return "west";
	case DirectionNorthEast:
		return "northeast";
	case DirectionNorthWest:
		return "northwest";
	case DirectionSouthEast:
		return "southeast";
	case DirectionSouthWest:
		return "northeast";
	default:
		return "unknown";
	};
}

Direction str_direction(std::string dir) {
	if(dir == "north" || dir == "n") {
		return DirectionNorth;
	}
	if(dir == "east" || dir == "e") {
		return DirectionEast;
	}
	if(dir == "south" || dir == "s") {
		return DirectionSouth;
	}
	if(dir == "west" || dir == "w") {
		return DirectionWest;
	}
	if(dir == "northeast" || dir == "ne") {
		return DirectionNorthEast;
	}
	if(dir == "northwest" || dir == "nw") {
		return DirectionNorthWest;
	}
	if(dir == "southeast" || dir == "se") {
		return DirectionSouthEast;
	}
	if(dir == "southwest" || dir == "sw") {
		return DirectionSouthWest;
	}
	return DirectionInvalid;
}
