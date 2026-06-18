#pragma once

#include "Question.h"
#include "Game.h"
#include "IDatabase.h"
#include "Room.h"
#include "StatisticsManager.h"
#include "RoomManager.h"


class GameManager
{
public:
	GameManager(IDatabase* db, RoomManager& roomManager);
	~GameManager();

	Game createGame(uint8_t roomId);
	void deleteGame(uint8_t gameId);
	LeaveGameStatus removePlayer(uint8_t gameId, const LoggedUser loggedUser);
	Question getQuestion(uint8_t gameId, const LoggedUser loggedUser);
	uint8_t submitAnswer(uint8_t gameId, const LoggedUser loggedUser, uint8_t answerId);
	uint8_t isGameOver(uint8_t gameId);
	std::vector<PlayerResults> getGameResults(uint8_t gameId);
	void submitGameStatsToDB(std::vector<PlayerResults>& playersResults, StatisticsManager statisticsManager);
	std::vector<Game> getGames();
	Game& getGame(uint8_t roomId);

private:
	IDatabase* m_database;
	RoomManager& m_roomManager;
	std::vector<Game> m_games;
};