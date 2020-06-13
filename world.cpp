#include <atomic>
#include <algorithm>
#include <thread>
#include <chrono>
#include "world.hpp"
#include "util.hpp"
#include "player.hpp"
#include "dir.hpp"
#include "room.hpp"
#include "object.hpp"

World::World() {
	this->self = std::shared_ptr<World>(this);
	this->startActorThread();
}

World::~World() {
	this->destructing = true;
	this->actorThread.join();
	this->nextID = 1; // start at 1, so any default-initialized ids are never valid.
}

ID World::NextID() {
	return this->nextID.fetch_add(1, std::memory_order_relaxed); // TODO verify order
}

void World::AddRoom(std::shared_ptr<Room> r) {
	this->rooms[r->ID] = r;
	this->room_links[r->ID] = std::map<Direction, RoomID>();
}

void World::AddPlayer(std::shared_ptr<Player> player, RoomID room_id) {
	this->players[player->ID] = player;
	this->name_players[player->Name] = player->ID;
	this->SetPlayerRoom(player->ID, room_id);
}

void World::AddObject(std::shared_ptr<Object> obj, PlayerID player_id) {
	this->objects[obj->ID] = obj;
	// TODO handle player not existing. Handle errors everywhere. Go has ruined me.
	std::shared_ptr<Player> player = this->GetPlayer(player_id);
	player->Objects[obj->ID] = obj;
	obj->LocationID.Player = player_id;
	obj->LocationType = ObjectLocationPlayer;
}


// LinkRooms links the two rooms in both direction, where the Direction is from room0 to room1.
// TODO add a special function for one-way paths.
void World::LinkRooms(RoomID room0, RoomID room1, Direction dir) {
	this->room_links[room0][dir] = room1;
	this->room_links[room1][reverse_direction(dir)] = room0;
}

void World::SetPlayerRoom(PlayerID playerID, RoomID newRoomID) {
	const auto oldPlayerRoomID = this->player_rooms.find(playerID);
	if(oldPlayerRoomID != this->player_rooms.end()) {

		auto playerSet = this->room_players.find(oldPlayerRoomID->second);
		playerSet->second.erase(playerID); // TODO check and error if world.room_players[room] doesn't exist (should never happen)
	}
	this->player_rooms[playerID] = newRoomID;
	this->room_players[newRoomID].insert(playerID);
}

void World::SetObjectRoom(ObjectID object_id, RoomID new_room_id) {
	std::shared_ptr<Object> obj = this->objects[object_id];

	switch(obj->LocationType) {
	case ObjectLocationRoom: {
		RoomID old_room_id = obj->LocationID.Room;
		std::shared_ptr<Room> old_room(this->rooms[old_room_id]);
		old_room->Objects.erase(object_id);
		break;
	}
	case ObjectLocationPlayer: {
		PlayerID player_id = obj->LocationID.Player;
		std::shared_ptr<Player> player = this->players[player_id];
		player->Objects.erase(object_id);
		break;
	}
	default: {
		// TODO handle error; should never happen
		return;
	}
	}

	std::shared_ptr<Room> room = this->rooms[new_room_id];
	room->Objects[object_id] = obj;
	obj->LocationType = ObjectLocationRoom;
	obj->LocationID.Room = new_room_id;

	// TODO print the move to players in previous and new room
}

void World::startActorThread() {
	// TODO change to something lighter weight than an OS thread
	// TBB? Microthread? Poll? Something else?
	this->actorThread = std::thread([=]() {
		for(;;) {
			if(this->destructing) {
				return;
			}
			for(auto it = this->objects.begin(), end = this->objects.end(); it != end; ++it) {
				std::shared_ptr<Object> obj = it->second;
				if(obj->Actor_ == nullptr) {
					continue;
				}
				obj->Actor_->Do(this->GetShared(), obj->ID);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(this->ActorThreadIntervalMS));
		}
	});
}

// GetPlayer returns the player with the given ID.
// If the ID doesn't exist, returns NULL.
std::shared_ptr<Player> World::GetPlayer(PlayerID v) {
	return this->players[v];
}

// GetRoom returns the room with the given ID.
// If the ID doesn't exist, returns NULL.
std::shared_ptr<Room> World::GetRoom(RoomID v) {
	return this->rooms[v];
}

// GetObject returns the object with the given ID.
// If the ID doesn't exist, returns NULL.
std::shared_ptr<Object> World::GetObject(ObjectID v) {
	return this->objects[v];
}

// Returns the room the given player is in.
RoomID World::GetPlayerRoom(PlayerID v) {
	// TODO change to return a bool? If a room didn't exist, it would be a subtle bug bug
	// But, players should always be in a room, right?
	return this->player_rooms[v];
}

std::map<Direction, RoomID> World::GetRoomLinks(RoomID v) {
	// TODO change to return a bool? If a room didn't exist, it would be a very subtle bug
	return this->room_links[v];
}

PlayerID World::GetPlayerByName(std::string name) {
		// TODO change to return a bool? If a player didn't exist, it would be a very subtle bug
	return this->name_players[name];
}

std::shared_ptr<World> World::GetShared() {
	return self;
}

LockedWorld::LockedWorld(LockableWorld w) {
	w.m->lock();
	this->w = w.w;
	this->l = w;
}

LockedWorld::~LockedWorld() {
	l.m->unlock();
}
