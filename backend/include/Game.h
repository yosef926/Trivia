#pragma once

#include <map>

#include "Question.h"
#include "LoggedUser.h"
#include "User.h"
#include "IRequestHandler.h"

struct GameData
{
	Question currentQuestion;
	uint8_t correctAnswerCount;
	uint8_t wrongAnswerCount;
	double averageAnswerTime;
};


class Game
{
public:
	Game(std::vector<Question> question, std::map<LoggedUser, GameData> players, uint8_t roomId);
	~Game();

	Question getQuestionForUser(const LoggedUser user);
	uint8_t submitAnswer(const LoggedUser loggedUser, uint8_t answerId);
	void removePlayer(const LoggedUser loggedUser);
	uint8_t getGameId();
	uint8_t getAmountOfQuestions();
	std::map<LoggedUser, GameData> getPlayers();
	std::vector<PlayerResults> getGameResults();
	uint8_t getRoomId();
	void updatePlayerTime(LoggedUser loggedUser, float newAvgAnswerTime);

private:
	std::vector<Question> m_questions;
	std::map<LoggedUser, GameData> m_players;
	uint8_t m_gameId;
	uint8_t m_roomId;
};