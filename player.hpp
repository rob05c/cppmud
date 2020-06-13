#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "enum.hpp"

struct IWorld;
struct MultiCommands;
struct Object;

struct Player {
	Player() {}; // TODO handle invalid lookups.
	Player(PlayerID id, std::string name, bool is_admin);

	std::string Do(std::shared_ptr<IWorld> world, std::string cmd);
	std::string Prompt(std::shared_ptr<IWorld> world);

	PlayerID    ID;
	std::string Name;
	bool        IsAdmin;

	// TODO abstract holding objects? An ObjectHolder might make it easy to atomically move objects
	std::map<ObjectID, std::shared_ptr<Object>> Objects;
private:
	std::shared_ptr<MultiCommands> commands;
};
