#include "cmd.hpp"
#include <algorithm>
#include <string>

#include "util.hpp"
#include "room.hpp"
#include "dir.hpp"
#include "world.hpp"
#include "player.hpp"
#include "object.hpp"

void MultiCommands::Add(std::string command_group, std::shared_ptr<Commands>& cmds) {
this->commands[command_group] = cmds;
}

std::tuple<std::string, bool> MultiCommands::Do(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	for(auto it = this->commands.begin(), end = this->commands.end(); it!=end; ++it) {
		std::tuple<std::string, bool> msg_handled = it->second->Do(world, player_id, cmds);
		if(std::get<1>(msg_handled) == true) {
			return msg_handled;
		}
	}
	return std::make_tuple("", false);
}

std::tuple<std::string, bool> GeneralCommands::Do(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	if(cmds.size() == 0) {
		return this->not_found(world, player_id, cmds);
	}
	if(cmds[0] == "look" || cmds[0] == "l" || cmds[0] == "ql" || cmds[0] == "quicklook") {
		return this->look(world, player_id, cmds);
	}
	if(cmds[0] == "get" || cmds[0] == "g") {
		return this->get(world, player_id, cmds);
	}
	if(cmds[0] == "drop" || cmds[0] == "d") {
		return this->drop(world, player_id, cmds);
	}
	if(cmds[0] == "inventory" || cmds[0] == "inv" || cmds[0] == "i" || cmds[0] == "ii") {
		return this->inventory(world, player_id, cmds);
	}
	if(str_direction(cmds[0]) != DirectionInvalid) {
		return this->move(world, player_id, cmds);
	}
	return this->not_found(world, player_id, cmds);
}

std::tuple<std::string, bool> GeneralCommands::not_found(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	return std::make_tuple("", false);
}

std::tuple<std::string, bool> GeneralCommands::look(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	const RoomID room_id = world->GetPlayerRoom(player_id);
	const std::shared_ptr<Room> room = world->GetRoom(room_id);
	const std::map<Direction, RoomID> room_links = world->GetRoomLinks(room_id);
	std::string str;
	str += room->Name;
	str += "\n";
	str += room->Description;
	str += "\n";
	str += "\n";
	std::vector<std::string> exit_strs;
	exit_strs.resize(room_links.size());
	std::transform(room_links.begin(), room_links.end(), exit_strs.begin(),
								 [](const std::pair<Direction, RoomID>& dir_room) -> std::string {
									 return direction_str(dir_room.first);
								 });

	std::vector<std::string> obj_strs;
	for(auto it = room->Objects.begin(), end = room->Objects.end(); it != end; ++it) {
		obj_strs.push_back(it->second->Clause);
	}

	if(obj_strs.size() == 1) {
		str += obj_strs[0] + " is here.\n";
	} else if(obj_strs.size() == 2) {
		str += obj_strs[0] + " and " + obj_strs[1] + " are here.\n";
	} else if(obj_strs.size() > 2) {
		const std::string last_obj = obj_strs.back();
		obj_strs.pop_back();
		str += util::join(obj_strs, ", ");
		str += ", and " + last_obj + " are here.\n";
	}

	if(exit_strs.size() == 0) {
		str += "There are no exits here."; // You mean impassible. Nothing is impossible.
	} else if(exit_strs.size() == 1) {
		str += "You see an exit to the " + exit_strs[0] + ".";
	} else if(exit_strs.size() == 2) {
		str += "You see exits to the " + exit_strs[0] + " and " + exit_strs[1] + ".";
	} else {
		str += "You see exits to the ";
		std::string last_exit = exit_strs[exit_strs.size()-1];
		exit_strs.pop_back();
		str += util::join(exit_strs, ", ");
		str += ", and " + last_exit + ".";
	}

	return make_tuple(str, true);
}

std::tuple<std::string, bool> GeneralCommands::inventory(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	std::shared_ptr<Player> player = world->GetPlayer(player_id);

	std::vector<std::string> obj_strs;
	for(auto it = player->Objects.begin(), end = player->Objects.end(); it != end; ++it) {
		obj_strs.push_back(it->second->Clause);
	}

	if(obj_strs.size() == 0) {
		const std::string msg = "You aren't carrying anything.\n";
		return make_tuple(msg, true);
	}

	std::string str = "You are carrying:\n";
	for(auto it = obj_strs.begin(), end = obj_strs.end(); it != end; ++it) {
		str += "  " + *it + "\n";
	}
	return make_tuple(str, true);
}

std::tuple<std::string, bool> GeneralCommands::move(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	const RoomID room_id = world->GetPlayerRoom(player_id);
	const std::shared_ptr<Room> room = world->GetRoom(room_id);
	const std::map<Direction, RoomID> room_links = world->GetRoomLinks(room_id);

	const Direction dir = str_direction(cmds[0]);

	auto new_room_id = room_links.find(dir);
	if(new_room_id == room_links.end()) {
		const std::string msg = "There is no exit in that direction.";
		return make_tuple(msg, true);
	}

	world->SetPlayerRoom(player_id, new_room_id->second);

	std::string msg = "You amble " + direction_str(dir) + ".";

	// TODO make showing the new room configurable
	msg += "\n";
	std::tuple<std::string, bool> look_msg_handled = this->look(world, player_id, cmds); // TODO assert handled=true
	msg += std::get<0>(look_msg_handled);

	return make_tuple(msg, true);
}

std::tuple<std::string, bool> GeneralCommands::drop(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	// TODO add drop by ID

	if(cmds.size() < 2) {
		const std::string msg = "What do you want to drop?";
		return make_tuple(msg, true);
	}
	const std::string obj_word = util::to_lower(cmds[1]);

	std::shared_ptr<Player> player = world->GetPlayer(player_id);
	std::shared_ptr<Object> obj;
	for(auto it = player->Objects.begin(), end = player->Objects.end(); it != end; ++it) {
		std::shared_ptr<Object> it_obj = it->second;
		if(util::to_lower(it_obj->Word) != obj_word) {
			continue;
		}
		obj = it_obj;
		break;
	}
	if(!obj) {
		const std::string msg = "You aren't carrying that.";
		return make_tuple(msg, true);
	}

	player->Objects.erase(obj->ID);

	const RoomID room_id = world->GetPlayerRoom(player_id);
	const std::shared_ptr<Room> room = world->GetRoom(room_id);
	room->Objects.insert(std::make_pair(obj->ID, obj));


	obj->LocationID.Room = room_id;
	obj->LocationType = ObjectLocationRoom;

	const std::string msg = "You yeet " + obj->Clause + ".";
	return make_tuple(msg, true);
}

std::tuple<std::string, bool> GeneralCommands::get(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	// TODO add drop by ID

	if(cmds.size() < 2) {
		const std::string msg = "What do you want to get?";
		return make_tuple(msg, true);
	}
	const std::string obj_word = util::to_lower(cmds[1]);

	std::shared_ptr<Player> player = world->GetPlayer(player_id);
	const RoomID room_id = world->GetPlayerRoom(player_id);
	const std::shared_ptr<Room> room = world->GetRoom(room_id);

	std::shared_ptr<Object> obj;
	for(auto it = room->Objects.begin(), end = room->Objects.end(); it != end; ++it) {
		std::shared_ptr<Object> it_obj = it->second;
		if(util::to_lower(it_obj->Word) != obj_word) {
			continue;
		}
		obj = it_obj;
		break;
	}
	if(!obj) {
		const std::string msg = "That isn't here.";
		return make_tuple(msg, true);
	}

	room->Objects.erase(obj->ID);
	player->Objects.insert(std::make_pair(obj->ID, obj));

	obj->LocationID.Player = player_id;
	obj->LocationType = ObjectLocationPlayer;

	const std::string msg = "You yoink " + obj->Clause + ".";
	return make_tuple(msg, true);
}

std::tuple<std::string, bool> AdminCommands::Do(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	if(cmds.size() == 0) {
		return this->not_found(world, player_id, cmds);
	}
	if(cmds[0] == "makeroom" || cmds[0] == "mr") {
		return this->make_room(world, player_id, cmds);
	} else if(cmds[0] == "describeroom" || cmds[0] == "dr") {
		return this->describe_room(world, player_id, cmds);
	} else if(cmds[0] == "makeobject" || cmds[0] == "makeobj" || cmds[0] == "mo") {
		return this->make_object(world, player_id, cmds);
	} else if(cmds[0] == "addaction" || cmds[0] == "aa") {
		return this->add_action(world, player_id, cmds);
	} else {
		return this->not_found(world, player_id, cmds);
	}
}

std::tuple<std::string, bool> AdminCommands::not_found(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	return std::make_tuple("", false);
}

std::tuple<std::string, bool> AdminCommands::make_room(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	if(cmds.size() < 2) {
		const std::string msg = "What direction do you want to create the room?";
		return std::make_tuple(msg, true);
	}
	if(cmds.size() < 3) {
		const std::string msg = "What do you want the name of the room to be?";
		return std::make_tuple(msg, true);
	}

	const Direction dir = str_direction(util::to_lower(cmds[1]));
	if(dir == DirectionInvalid) {
		const std::string msg = "I don't know where that direction is.";
		return std::make_tuple(msg, true);
	}

	RoomID player_room_id = world->GetPlayerRoom(player_id);
	std::shared_ptr<Room> player_room = world->GetRoom(player_room_id);

	std::map<Direction, RoomID> player_room_links = world->GetRoomLinks(player_room_id);
	if(player_room_links.find(dir) !=  player_room_links.end()) {
		const std::string msg = "You gesture forcefully " + direction_str(dir) + ", but the reality there refuses to abdicate.";
		return std::make_tuple(msg, true);
	}

	std::vector<std::string> room_name_words;
	for(int i = 2; i < cmds.size(); ++i) {
		room_name_words.push_back(cmds[i]);
	}
	const std::string room_name = util::join(room_name_words, " ");

	std::shared_ptr<Room> room = std::shared_ptr<Room>(new Room());
	room->ID = world->NextID();
	room->Name = room_name;
	room->Description = "A white mist pervades the area.";
	world->AddRoom(room);
	world->LinkRooms(player_room->ID, room->ID, dir);

	const std::string msg = "The " + direction_str(dir) + " coalesces from ether.";
	return std::make_tuple(msg, true);
}

std::tuple<std::string, bool> AdminCommands::describe_room(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	RoomID player_room_id = world->GetPlayerRoom(player_id);
	std::shared_ptr<Room> player_room = world->GetRoom(player_room_id);

	std::vector<std::string> room_desc_words;
	for(int i = 1; i < cmds.size(); ++i) {
		room_desc_words.push_back(cmds[i]);
	}
	const std::string room_desc = util::join(room_desc_words, " ");
	player_room->Description = room_desc;

	const std::string msg = "The void recedes.";
	return std::make_tuple(msg, true);
}

std::tuple<std::string, bool> AdminCommands::make_object(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	if(cmds.size() < 2) {
		const std::string msg = "What kind of thing is the object?";
		return std::make_tuple(msg, true);
	} else if (cmds.size() < 3) {
		const std::string msg = "What does the object look like?";
		return std::make_tuple(msg, true);
	}

	const std::string obj_name = cmds[1];

	std::vector<std::string> obj_desc_words;
	for(int i = 2; i < cmds.size(); ++i) {
		obj_desc_words.push_back(cmds[i]);
	}

	std::shared_ptr<Object> obj = std::shared_ptr<Object>(new Object());
	obj->ID = world->NextID();
	obj->Word = obj_name;
	obj->Clause = util::join(obj_desc_words, " ");
	obj->Paragraph = "This object is an indescribable amorphous blob.";
	obj->Ground = "An indescribable amorphous blob lies here.";

	world->AddObject(obj, player_id);

	const std::string msg = obj->Clause + " materializes in your hands."; // TODO capitalize clause; add capitalize util.
	return std::make_tuple(msg, true);
}

std::tuple<std::string, bool> AdminCommands::add_action(std::shared_ptr<IWorld> world, PlayerID player_id, std::vector<std::string> cmds) {
	if(cmds.size() < 2) {
		const std::string msg = "What object do you want to add an action to?";
		return std::make_tuple(msg, true);
	} else if (cmds.size() < 3) {
		const std::string msg = "What action do you want to add to the object? Possible action is: meander.";
		return std::make_tuple(msg, true);
	}

	const std::string obj_word = cmds[1];

	std::shared_ptr<Player> player = world->GetPlayer(player_id);
	std::shared_ptr<Object> obj;
	for(auto it = player->Objects.begin(), end = player->Objects.end(); it != end; ++it) {
		std::shared_ptr<Object> it_obj = it->second;
		if(util::to_lower(it_obj->Word) != obj_word) {
			continue;
		}
		obj = it_obj;
		break;
	}
	if(!obj) {
		const std::string msg = "You aren't carrying that.";
		return make_tuple(msg, true);
	}

	const std::string action_name = util::to_lower(cmds[2]);
	if(action_name == "meander") {
		std::shared_ptr<ActorDoFunctor> action(new ActorDoFunctorMeander);
		if(obj->Actor_ == nullptr) {
			obj->Actor_.reset(new Actor());
		}
		obj->Actor_->DoFunctors.push_back(action);

		const std::string msg = obj->Clause + " will now " + action_name;
		return std::make_tuple(msg, true);
	} else {
		const std::string msg = "That sounds hard.";
		return std::make_tuple(msg, true);
	}
}
