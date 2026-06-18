#include "RoomManager.h"

RoomManager::RoomManager()
{
}

RoomManager::~RoomManager()
{
}

CreateRoomStatus RoomManager::createRoom(const LoggedUser loggedUser, const RoomData& roomData)
{
	try
	{
		Room newRoom(roomData);
		newRoom.addUser(loggedUser);
		m_rooms.insert({ roomData.id, newRoom });
		std::cout << "[NEW ROOM CREATED, ID]: " << static_cast<int>(roomData.id) << std::endl;
		return CreateRoomStatus::SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << "RoomManager Error: " << e.what() << std::endl;
		return CreateRoomStatus::FAIL;
	}
}

CloseRoomStatus RoomManager::deleteRoom(uint8_t id)
{
	size_t res = m_rooms.erase(id);

	if (!res) {
		return CloseRoomStatus::FAIL;
	}
	return CloseRoomStatus::SUCCESS;
}

RoomStatus RoomManager::getRoomState(uint8_t id)
{
	for (size_t i = 0; i < m_rooms.size(); i++)
	{
		RoomData roomData = m_rooms.at(i).getRoomData();
		if (roomData.id == id)
		{
			return roomData.status;
		}
	}
	throw std::runtime_error(std::string("No room with id ") + std::to_string(id) + std::string(" has found"));
}

std::vector<RoomData> RoomManager::getRooms()
{
	std::vector<RoomData> roomsData;
	for (auto& pair : m_rooms)
	{
		RoomData roomData = pair.second.getRoomData();
		if (roomData.status == RoomStatus::WAITING_FOR_PLAYERS || roomData.status == RoomStatus::RUNNING)
			roomsData.push_back(roomData);
	}
	return roomsData;
}

std::optional<std::reference_wrapper<Room>> RoomManager::getRoom(uint8_t id)
{
	auto it = m_rooms.find(id);
	if (it != m_rooms.end())
	{
		return it->second;
	}
	return std::nullopt;
}

uint8_t RoomManager::findAvailableRoomId()
{
	uint8_t generatedId;
	do
	{
		generatedId = Helper::generateId();	
	} while (m_rooms.find(generatedId) != m_rooms.end()); // as long as this id exists in map, keep searching for one that is not(unique id).
	return generatedId;
}