#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "IRequestHandler.h"
#include "LoggedUser.h"

struct RoomData
{
	uint8_t id;
	std::string name;
	uint8_t maxPlayers;
	uint8_t numOfQuestionsInGame;
	uint8_t timePerQuestions;
	RoomStatus status;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RoomData, id, name, maxPlayers, numOfQuestionsInGame, timePerQuestions, status);

class Room
{
public:
	Room(const RoomData& roomData);
	~Room();

	JoinRoomStatus addUser(const LoggedUser loggedUser);
	LeaveRoomStatus removeUser(const LoggedUser loggedUser);
	std::vector<std::string> getAllUsers();

	RoomData getRoomData() const;
	void changeStatus(RoomStatus roomStatus);
	std::vector<LoggedUser> getAllLoggedUsers() const; // Same as getAllUsers, just here I get the objects themselves

private:
	RoomData m_metadata;
	std::vector<LoggedUser> m_users;
};