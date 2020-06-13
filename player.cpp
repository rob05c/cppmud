#include "player.hpp"
#include "cmd.hpp"
#include "util.hpp"

Player::Player(PlayerID id, std::string name, bool is_admin) {
	ID = id;
	Name = name;
	IsAdmin = is_admin;

	commands.reset(new MultiCommands);
	auto general_cmds = std::shared_ptr<Commands>(new GeneralCommands);
	auto admin_cmds = std::shared_ptr<Commands>(new AdminCommands);
	commands->Add("general", general_cmds);
	if(IsAdmin) {
		commands->Add("admin", admin_cmds);
	}
}

std::string Player::Do(std::shared_ptr<IWorld> world, std::string cmd) {
	std::vector<std::string> cmds = util::split(cmd, " ");
	std::tuple<std::string, bool> msg_handled = this->commands->Do(world, this->ID, cmds);
	if(std::get<1>(msg_handled) == false) {
		return "I don't understand.";
	}
	return std::get<0>(msg_handled);
}

std::string Player::Prompt(std::shared_ptr<IWorld> world) {
	return ">";
}
