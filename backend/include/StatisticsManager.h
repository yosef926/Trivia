#pragma once

#include <vector>
#include <string>

#include "IDatabase.h"

class StatisticsManager
{
public:
	StatisticsManager(IDatabase* db);
	~StatisticsManager();

	std::vector<std::string> getHighScores();
	std::vector<std::string> getUserStatistics(const std::string& username);
	int calcScore(PlayerResults userStatistics);

private:
	IDatabase* m_database;
};