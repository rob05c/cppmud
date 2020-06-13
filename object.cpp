#include <cstdlib>
#include "object.hpp"
#include "dir.hpp"
#include "world.hpp"

void Actor::Do(std::shared_ptr<World> world, ObjectID object_id) {
	for(auto it = this->DoFunctors.begin(), end = this->DoFunctors.end(); it != end; ++it) {
		(*it)->Do(world, object_id);
	}
}

void ActorDoFunctorMeander::Do(std::shared_ptr<World> world, ObjectID object_id) {
	const uint meander_interval_ms = 10000;
	this->interval++;
	// only meander around every 10 seconds
	const uint executeTickMod = meander_interval_ms / world->ActorThreadIntervalMS;
	if(this->interval % executeTickMod != 0) {
		return;
	}

	std::shared_ptr<Object> obj = world->GetObject(object_id);
	// TODO check for null? should never happen

	if(obj->LocationType != ObjectLocationRoom) {
		return; // not in a room, can't move
	} else {
	}

	auto room_id = obj->LocationID.Room;

	auto room_links = world->GetRoomLinks(room_id);

	if(room_links.size() == 0) {
		return; // nowhere to move
	}

	std::vector<Direction> dirs;
	for(auto it = room_links.begin(), end = room_links.end(); it != end; ++it) {
		dirs.push_back(it->first);
	}

	int rand_dir_i = rand() % dirs.size();
	Direction dir = dirs[rand_dir_i];
	RoomID new_room_id = room_links[dir];

	world->SetObjectRoom(object_id, new_room_id);

	// TODO broadcast entry/exit to rooms
};
