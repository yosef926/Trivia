#include "Game.h"
#include "Helper.h"

Game::Game(std::vector<Question> question, std::map<LoggedUser, GameData> players, uint8_t roomId)
	: m_questions(question),
	  m_players(players),
	  m_gameId(Helper::generateId()),
	  m_roomId(roomId)
{
}

Game::~Game()
{
}

Question Game::getQuestionForUser(const LoggedUser user)
{
	GameData& data = m_players.at(user);
	Question current = data.currentQuestion;

	for (size_t i = 0; i < m_questions.size(); i++)
	{
		if (m_questions.at(i) == data.currentQuestion)
		{
			if (i + 1 < m_questions.size())
			{
				data.currentQuestion = m_questions.at(i + 1);
			}
			return current;
		}
	}
	return current;
}

uint8_t Game::submitAnswer(const LoggedUser loggedUser, uint8_t answerId)
{
	uint8_t correctAnswerId = m_players.at(loggedUser).currentQuestion.getCorrectAnswerId();
	
	m_players.at(loggedUser).correctAnswerCount += (answerId == correctAnswerId);
	m_players.at(loggedUser).wrongAnswerCount += !(answerId == correctAnswerId);
	
	return correctAnswerId;
}

void Game::removePlayer(const LoggedUser loggedUser)
{
	m_players.erase(loggedUser);
}

uint8_t Game::getGameId()
{
	return m_gameId;
}

uint8_t Game::getAmountOfQuestions()
{
	return static_cast<uint8_t>(m_questions.size());
}

std::map<LoggedUser, GameData> Game::getPlayers()
{
	return m_players;
}

std::vector<PlayerResults> Game::getGameResults()
{
	std::vector<PlayerResults> playersResults;

	for (const std::pair<LoggedUser, GameData>& user : m_players)
	{
		PlayerResults playerResult = { user.first.getUsername(), user.second.correctAnswerCount, user.second.wrongAnswerCount, std::round((user.second.averageAnswerTime) * 100.0f) / 100.0f };
		playersResults.push_back(playerResult);
	}
	return playersResults;
}

uint8_t Game::getRoomId()
{
	return m_roomId;
}

void Game::updatePlayerTime(LoggedUser loggedUser, float newAvgAnswerTime)
{
	GameData gameData = m_players.at(loggedUser);
	gameData.averageAnswerTime = newAvgAnswerTime;
	m_players.at(loggedUser) = gameData;
}
