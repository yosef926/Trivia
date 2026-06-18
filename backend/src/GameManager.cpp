#include "GameManager.h"
#include "SqliteDataBase.h"

GameManager::GameManager(IDatabase* db, RoomManager& roomManager)
	: m_database(db),
	  m_roomManager(roomManager)
{
}

GameManager::~GameManager()
{
}

Game GameManager::createGame(uint8_t roomId)
{
	auto roomOpt = m_roomManager.getRoom(roomId);
	if (!roomOpt)
	{
		throw std::runtime_error("Room not found");
	}

	// Randmoly get questions from DB to create Game from DB
	std::vector<Question> questions = m_database->getQuestions(roomOpt.value().get().getRoomData().numOfQuestionsInGame);
	
	std::map<LoggedUser, GameData> players;

	// Default GameData(initialize, set currentQuestion to first question for all users in the game)
	GameData gameData = { questions.at(0), 0, 0, 0};

	std::vector<LoggedUser> users = roomOpt.value().get().getAllLoggedUsers();
	for (LoggedUser user: users)
	{
		players.insert_or_assign(user, gameData);
	}

	Game game(questions, players, roomId);
	m_games.push_back(game);
	return game;
}

void GameManager::deleteGame(uint8_t gameId)
{
	for (size_t i = 0; i < m_games.size(); i++)
	{
		if (m_games.at(i).getGameId() == gameId)
		{
			m_games.erase(m_games.begin() + i);
		}
	}
}

LeaveGameStatus GameManager::removePlayer(uint8_t gameId, const LoggedUser loggedUser)
{
	LeaveGameStatus status = LeaveGameStatus::FAIL;
	for (Game& game : m_games)
	{
		if (game.getGameId() == gameId)
		{
			game.removePlayer(loggedUser);
			status = LeaveGameStatus::SUCCESS;
		}
	}
	return status;
}

Question GameManager::getQuestion(uint8_t gameId, const LoggedUser loggedUser)
{
	for (Game& game : m_games)
	{
		if (game.getGameId() == gameId)
		{
			return game.getQuestionForUser(loggedUser);
		}
	}
}

uint8_t GameManager::submitAnswer(uint8_t gameId, const LoggedUser loggedUser, uint8_t answerId)
{
	for (Game& game : m_games)
	{
		if (game.getGameId() == gameId)
		{
			return game.submitAnswer(loggedUser, answerId);
		}
	}
	throw std::runtime_error("Answer id: " + std::to_string(answerId) + " is not in list of answers");
}

uint8_t GameManager::isGameOver(uint8_t gameId)
{
	int amountOfQuestions = 0;

	for (Game& game : m_games)
	{
		if (game.getGameId() == gameId)
		{
			amountOfQuestions = static_cast<int>(game.getAmountOfQuestions());
			for (const std::pair<LoggedUser, GameData>& player : game.getPlayers())
			{
				if ((static_cast<int>(player.second.correctAnswerCount) + static_cast<int>(player.second.wrongAnswerCount) < amountOfQuestions))
				{
					return false;
				}
			}
		}
	}
	return true;
}

std::vector<PlayerResults> GameManager::getGameResults(uint8_t gameId)
{
	std::vector<PlayerResults> playersResults;
	for (Game& game : m_games)
	{
		if (game.getGameId() == gameId)
		{
			playersResults = game.getGameResults();
		}
	}
	return playersResults;
}

void GameManager::submitGameStatsToDB(std::vector<PlayerResults>& playersResults, StatisticsManager statisticsManager)
{
	for (PlayerResults& playerResults : playersResults)
	{
		std::vector<std::string> userStatistics = statisticsManager.getUserStatistics(playerResults.username);
		m_database->updatePlayerStats(playerResults, userStatistics, statisticsManager.calcScore(playerResults));
	}
}

std::vector<Game> GameManager::getGames()
{
	return m_games;
}

Game& GameManager::getGame(uint8_t roomId)
{
	for (Game& game: m_games)
	{
		if (game.getRoomId() == roomId)
		{
			return game;
		}

	}
}
