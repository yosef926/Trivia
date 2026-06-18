#include "Room.h"
#include "IRequestHandler.h"

Room::Room(const RoomData& roomData)
	: m_metadata(roomData)
{
}

Room::~Room()
{
}

JoinRoomStatus Room::addUser(const LoggedUser loggedUser)
{
	JoinRoomStatus res = JoinRoomStatus::FAIL;
	if (m_users.size() < m_metadata.maxPlayers)
	{
		m_users.push_back(loggedUser);
		res = JoinRoomStatus::SUCCESS;
	}
	return res;
}

LeaveRoomStatus Room::removeUser(const LoggedUser loggedUser)
{
	LeaveRoomStatus res = LeaveRoomStatus::FAIL;
	for (size_t i = 0; i < m_users.size(); i++)
	{
		if (loggedUser == m_users.at(i))
		{
			m_users.erase(m_users.begin() + i);
			res = LeaveRoomStatus::SUCCESS;
		}
	}
	return res;
}

std::vector<std::string> Room::getAllUsers()
{
	std::vector<std::string> allUsers;

	for (size_t i = 0; i < m_users.size(); i++)
	{
		allUsers.push_back(m_users.at(i).getUsername());
	}
	return allUsers;
}

RoomData Room::getRoomData() const
{
	return m_metadata;
}

void Room::changeStatus(RoomStatus roomStatus)
{
	m_metadata.status = roomStatus;
}

std::vector<LoggedUser> Room::getAllLoggedUsers() const
{
	return m_users;
}
