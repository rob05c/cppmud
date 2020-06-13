#pragma once

#include <string>
#include <vector>

#include "enum.hpp"

struct Object;

struct Room {
	RoomID      ID;
	std::string Name;
	std::string Description;

	std::map<ObjectID, std::shared_ptr<Object>> Objects;
};
