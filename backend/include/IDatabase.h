#pragma once

#include <variant>
#include <iostream>
#include <string>
#include <list>

#include "Question.h"
#include "IRequestHandler.h"

using Cell = std::variant<int, double, std::string>;
using Row = std::vector<Cell>;
using QueryResult = std::vector<Row>;

class IDatabase
{
public:
	virtual ~IDatabase() = default;

	virtual bool open() = 0;
	virtual bool close() = 0;

	// *** Queries ***
	virtual int doesUserExist(const std::string& username) = 0;
	virtual int doesPasswordMatch(const std::string& username, const std::string& password) = 0;
	
	// statistics Queries
	virtual double getPlayerAverageAnswerTime(const std::string& username) = 0;
	virtual int getNumOfCorrectAnswers(const std::string& username) = 0;
	virtual int getNumOfTotalAnswers(const std::string& username) = 0;
	virtual int getNumOfPlayerGames(const std::string& username) = 0;
	virtual int getPlayerScore(const std::string& username) = 0;
	virtual std::vector<std::string> getHighScores() = 0;
	virtual int updatePlayerStats(PlayerResults playerResults, std::vector<std::string> userStatistics, int score) = 0;

	// Questions
	virtual std::vector<Question> getQuestions(int amountOfQuestions) = 0;

	// Operations
	virtual int addNewUser(const std::string& username, const std::string& password, const std::string& email) = 0;

private:
};