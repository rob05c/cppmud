#ifndef enumhpp
#define enumhpp

// TODO rename file to just "enum"

// This should be the one file allowed to be included by headers

typedef uint64_t ID;
typedef ID RoomID;
typedef ID PlayerID;
typedef ID ObjectID;

enum Direction {
	DirectionNorth,
	DirectionEast,
	DirectionSouth,
	DirectionWest,
	DirectionNorthEast,
	DirectionNorthWest,
	DirectionSouthEast,
	DirectionSouthWest,
	DirectionInvalid = 255
};

enum ObjectLocation {
	ObjectLocationRoom,
	ObjectLocationPlayer,
	ObjectLocationInvalid = 255
};

#endif
