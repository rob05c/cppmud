#pragma once

#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include <vector>

#include "enum.hpp"

struct IWorld;

// Commands is an interface for a group of commands.
// A group of commands might be: generic actions like move and look, admin commands like create, or player class like wizard.
struct Commands {
	virtual std::tuple<std::string, bool> Do(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) = 0;
	virtual ~Commands(){};
};

struct MultiCommands : Commands {
	MultiCommands(){};
	// virtual MultiCommands(MultiCommands const&){}; // TODO implement?
	~MultiCommands(){};

	// each group needs a name so it can be removed later. E.g. if a player changes class, or has admin revoked.
	void Add(std::string command_group, std::shared_ptr<Commands>& commands);
	virtual std::tuple<std::string, bool> Do(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
private:
	std::map<std::string, std::shared_ptr<Commands>> commands;
};

struct GeneralCommands : Commands {
	virtual ~GeneralCommands(){};
	virtual std::tuple<std::string, bool> Do(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
private:
	std::tuple<std::string, bool> not_found(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> look(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> move(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> get(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> drop(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> inventory(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
};

struct AdminCommands : Commands {
	virtual ~AdminCommands(){};
	virtual std::tuple<std::string, bool> Do(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
private:
	std::tuple<std::string, bool> make_room(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> not_found(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> describe_room(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> make_object(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
	std::tuple<std::string, bool> add_action(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds);
};
