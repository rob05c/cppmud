#pragma once

#include <thread>
#include <vector>
#include "enum.hpp"

class Actor;
class World;
class ActorDoFunctor;

union ObjectLocationID {
	RoomID Room;
	PlayerID Player;
};

class Object {
public:
	ObjectID    ID;
	std::string Word;        // word; used to interact, e.g. "ball"
	std::string Clause;  // noun clause; used in places like inventories, semi-unique, e.g. "a blue ball"
	std::string Paragraph; // paragraph; used when inspecting, e.g. "This ball is Ugly. So ugly you should drop it. ..."
	std::string Ground;      // sentence; used when the item is on the ground, e.g. "A blue ball lies here, depreciating."

	ObjectLocationID LocationID;
	ObjectLocation LocationType;

	std::shared_ptr<Actor> Actor_; // will be nullptr, if the object has no actions (i.e. most items vs NPCs)
};

class ActorDoFunctor;

// Actor is the functor that controls the behavior of an Object (usually an NPC)
class Actor {
public:
	// TODO add broadcast mechanism. The world needs to be able to broadcast messages to all players when an object does something. Or another player,  or the environment (which may end up being invisible objects.

	// Do is executed on every tick.
	// TODO figure out how often a tick is? Or should this pick how often it runs? Async? State?
	// TODO figure out when Do executes?
	//      - all objects ever? That doesn't seem reasonable
	//      - all objects on the ground? What if we want a mechanism to execute in someone's inventory? Does that make sense?
	//      - objects on the ground, with a player in the local area? If so, should objects fake catching up?
	//        - How does fake catchup handle object interactions? Should we just spam so many ticks?
	//          Is that more performant than always executing anyway?
	void Do(std::shared_ptr<World> world, ObjectID object_id);

	// DoFunctors are executed by Do (which should be called every tick) in order.
	std::vector<std::shared_ptr<ActorDoFunctor>> DoFunctors;
};

class ActorDoFunctor {
public:
	virtual void Do(std::shared_ptr<World> world, ObjectID object_id) = 0;
};

// ActorDoFunctorMeander makes the object walk around randomly.
class ActorDoFunctorMeander : public ActorDoFunctor {
public:
	virtual void Do(std::shared_ptr<World> world, ObjectID object_id) override;
private:
	uint interval;
};
