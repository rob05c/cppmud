#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <memory>

#include "repl.hpp"
#include "util.hpp"
#include "world.hpp"
#include "room.hpp"
#include "player.hpp"

std::shared_ptr<World> makeWorld();

int repl() {
	std::shared_ptr<World> raw_world = makeWorld();
	LockableWorld lockable_world(raw_world);
	PlayerID player_id = raw_world->GetPlayerByName("bob");

	{
		std::shared_ptr<IWorld> world((IWorld*)new LockedWorld(lockable_world));
		std::shared_ptr<Player> player = world->GetPlayer(player_id);
		printf("Welcome!\n");
		printf("%s ", player->Prompt(world).c_str());
	}

  for (std::string line; std::getline(std::cin, line);) {
		const std::string line_lower = util::to_lower(line);
		if (line_lower == "quit" || line_lower == "q" || line_lower == "exit") {
			printf("Goodbye!\n");
			return 0;
		}

		std::shared_ptr<IWorld> world((IWorld*) new LockedWorld(lockable_world));
		std::shared_ptr<Player> player = world->GetPlayer(player_id);
		printf("%s\n", player->Do(world, line).c_str());
		printf("%s ", player->Prompt(world).c_str());
	}
	return 1;
}

// make initial debug world
std::shared_ptr<World> makeWorld() {
	std::shared_ptr<World> world = (new World)->GetShared();

	std::shared_ptr<Room> room0 = std::shared_ptr<Room>(new Room());
	room0->ID = world->NextID();
	room0->Name = "A small kitchen.";
	room0->Description = "This small kitchen is filled with yellow and white flowers.";
	world->AddRoom(room0);

	std::shared_ptr<Room> room1 = std::shared_ptr<Room>(new Room());
	room1->ID = world->NextID();
	room1->Name = "A large garden.";
	room1->Description = "This large garden has many topiaries of all kinds of animals. In one corner, a hedge suspiciously covers a wall.";
	world->AddRoom(room1);

	world->LinkRooms(room0->ID, room1->ID, DirectionWest);

	std::shared_ptr<Player> player0 = std::shared_ptr<Player>(new Player(world->NextID(), "alice", true));
	world->AddPlayer(player0, room0->ID);

	std::shared_ptr<Player> player1 = std::shared_ptr<Player>(new Player(world->NextID(), "bob", true));
	world->AddPlayer(player1, room0->ID);

	return world;
}
