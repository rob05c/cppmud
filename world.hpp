#pragma once

#include <mutex>
#include <cstdint>
#include <set>
#include <map>
#include <vector>
#include <tuple>
#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "enum.hpp"

struct Player;
struct Room;
struct Object;

// TODO user pointers instead of IDs? Faster lookups?

// TODO add msg broadcast funcs (player, room, area, etc)


// TODO figure out how to orchestrate locking.
// Phase 1: lock the world

// Phase 2: lock per-room
// How to figure out what's in a room? I.e. you're not allowed to even read objects in a room

// - Put objects actually as members of the room?
// - What about room links?

// Phase 2.x: lock player items??

class IWorld {
public:
	virtual ~IWorld() {};

	virtual ID                          NextID() = 0;
	virtual void                        LinkRooms(RoomID a, RoomID b, Direction d) = 0;
	virtual void                        SetPlayerRoom(PlayerID p, RoomID r) = 0;
	virtual void                        SetObjectRoom(ObjectID o, RoomID r) = 0;
	virtual void                        AddRoom(std::shared_ptr<Room> r) = 0;
	virtual void                        AddPlayer(std::shared_ptr<Player> p, RoomID r) = 0;
	virtual void                        AddObject(std::shared_ptr<Object> o, PlayerID p) = 0;
	virtual std::shared_ptr<Room>       GetRoom(RoomID v) = 0;
	virtual std::shared_ptr<Player>     GetPlayer(PlayerID v) = 0;
	virtual std::shared_ptr<Object>     GetObject(ObjectID v) = 0;
	virtual std::map<Direction, RoomID> GetRoomLinks(RoomID v) = 0;
	virtual RoomID                      GetPlayerRoom(PlayerID v) = 0;
	virtual PlayerID                    GetPlayerByName(std::string) = 0;
};


// TODO lock literally everything everytime anything happens.
struct World : IWorld {
	const uint ActorThreadIntervalMS = 2000;

	World();
	~World();

	// This should be ok locking independently, because rooms should never be deleted with anything happening in them.
	// TODO make sure there's a trylock in std? Need to dining-philosophers with the room-stuff?

	// RoomLinks is guaranteed to have an entry for every entry in Rooms.

	ID NextID() override;
	void LinkRooms(RoomID, RoomID, Direction) override;
	void SetPlayerRoom(PlayerID, RoomID) override;
	void SetObjectRoom(ObjectID, RoomID) override;

	// TODO change add funcs to not require id, but to set it themselves. World should be responsible for generating IDS
	void AddRoom(std::shared_ptr<Room>) override;
	void AddPlayer(std::shared_ptr<Player>, RoomID) override;
	void AddObject(std::shared_ptr<Object>, PlayerID) override;

	std::shared_ptr<Room>       GetRoom(RoomID v) override;
	std::shared_ptr<Player>     GetPlayer(PlayerID v) override;
	std::shared_ptr<Object>     GetObject(ObjectID v) override;
	std::map<Direction, RoomID> GetRoomLinks(RoomID v) override;
	RoomID                      GetPlayerRoom(PlayerID v) override;
	PlayerID                    GetPlayerByName(std::string) override;
	std::shared_ptr<World>      GetShared();
protected:
	World(const World& w); // noncopyable

	std::shared_ptr<World> self;

	void startActorThread();

	std::atomic<ID> nextID;
	std::thread actorThread;
	bool destructing;

	// This should be ok locking independently, because players should never be deleted by room actions
	std::map<PlayerID, std::shared_ptr<Player>>   players;
	std::map<RoomID, std::shared_ptr<Room>>       rooms;
	std::map<RoomID, std::map<Direction, RoomID>> room_links;

	// lookup maps. Almost any time you're iterating, you should make a lookup map instead
	std::map<PlayerID, RoomID> player_rooms;
	std::map<RoomID, std::set<PlayerID>> room_players;
	std::map<std::string, PlayerID> name_players;
	std::map<ObjectID, std::shared_ptr<Object>> objects;
};

class LockedWorld;

// LockableWorld has a world which may be locked to access.
// To use, create a LockedWorld object.
//
// Should be copied. Contains pointers to members.
//
class LockableWorld {
public:
	LockableWorld(std::shared_ptr<World> _w)
		: w(_w)
		, m(new std::mutex)
	{}
	~LockableWorld() {};

	LockableWorld(const LockableWorld& n)
		: w(n.w)
		, m(n.m)
	{}

protected:
	friend class LockedWorld;

	LockableWorld() {};       // non-default-constructable

	std::shared_ptr<World> w;
	std::shared_ptr<std::mutex> m;
};

// TODO Add ReadLockedWorld?
//      Gonna be a pain to guarantee no modification.
//      Would require readonly interfaces for every object type.
//      Maybe it's not that bad?

// LockedWorld is an object with all the functions of World, which locks the world on creation, and releases the lock on destruction.
//
// Should be copied. Contains pointers to members.
//
class LockedWorld : IWorld {
public:
	LockedWorld(LockableWorld w);
	~LockedWorld();

	ID                          NextID()                                         {return w->NextID();}
	void                        LinkRooms(RoomID a, RoomID b, Direction d)       {return w->LinkRooms(a,b,d);}
	void                        SetPlayerRoom(PlayerID p, RoomID r)              {return w->SetPlayerRoom(p,r);}
	void                        SetObjectRoom(ObjectID o, RoomID r)              {return w->SetObjectRoom(o,r);}
	void                        AddRoom(std::shared_ptr<Room> r)                 {return w->AddRoom(r);}
	void                        AddPlayer(std::shared_ptr<Player> p, RoomID r)   {return w->AddPlayer(p,r);}
	void                        AddObject(std::shared_ptr<Object> o, PlayerID p) {return w->AddObject(o,p);}
	std::shared_ptr<Room>       GetRoom(RoomID v)                                {return w->GetRoom(v);}
	std::shared_ptr<Player>     GetPlayer(PlayerID v)                            {return w->GetPlayer(v);}
	std::shared_ptr<Object>     GetObject(ObjectID v)                            {return w->GetObject(v);}
	std::map<Direction, RoomID> GetRoomLinks(RoomID v)                           {return w->GetRoomLinks(v);}
	RoomID                      GetPlayerRoom(PlayerID v)                        {return w->GetPlayerRoom(v);}
	PlayerID                    GetPlayerByName(std::string v)                   {return w->GetPlayerByName(v);}
	std::shared_ptr<World>      GetShared();
protected:
	LockedWorld();                     // non-default-constructable
	LockedWorld(const LockedWorld& w); // noncopyable

	LockableWorld l;
	std::shared_ptr<World> w;
};

struct PlayerContext {
	PlayerID PlayerID_;
	std::shared_ptr<World> World_;
};

std::string ProcessCmd(std::shared_ptr<World> world, PlayerID player_id, std::string cmd);
