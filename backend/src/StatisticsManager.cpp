#include "StatisticsManager.h"

StatisticsManager::StatisticsManager(IDatabase* db)
	: m_database(db)
{
}

StatisticsManager::~StatisticsManager()
{
}


std::vector<std::string> StatisticsManager::getHighScores()
{
	return m_database->getHighScores();
}


std::vector<std::string> StatisticsManager::getUserStatistics(const std::string& username)
{
	std::vector<std::string> userStatistics;
	userStatistics.push_back(std::to_string(m_database->getPlayerAverageAnswerTime(username)));
	userStatistics.push_back(std::to_string(m_database->getNumOfCorrectAnswers(username)));
	userStatistics.push_back(std::to_string(m_database->getNumOfTotalAnswers(username)));
	userStatistics.push_back(std::to_string(m_database->getNumOfPlayerGames(username)));
	userStatistics.push_back(std::to_string(m_database->getPlayerScore(username)));

	return userStatistics;
}

int StatisticsManager::calcScore(PlayerResults playerResults)
{
	double avgAnswerTime = playerResults.averageAnswerTime;
	int numCorrectAnswers = playerResults.correctAnswerCount;

	if (avgAnswerTime < 0.1)
	{
		avgAnswerTime = 0.1;
	}

	int score = (numCorrectAnswers * 100) + static_cast<int>(numCorrectAnswers * (10.0 / avgAnswerTime));

	return score;
}