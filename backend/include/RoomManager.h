#pragma once

#include <optional>
#include <map>
#include <functional>

#include "Room.h"
#include "LoggedUser.h"
#include "Constants.h"
#include "Helper.h"

class RoomManager
{
public:
	RoomManager();
	~RoomManager();
	
	CreateRoomStatus createRoom(const LoggedUser loggedUser, const RoomData& roomData);
	CloseRoomStatus deleteRoom(uint8_t id);
	RoomStatus getRoomState(uint8_t id);
	std::vector<RoomData> getRooms();
	std::optional<std::reference_wrapper<Room>> getRoom(uint8_t id);
	uint8_t findAvailableRoomId();

private:
	std::unordered_map<uint8_t, Room> m_rooms; // The uint8_t key is the id of the room
};