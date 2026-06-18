#pragma once

#include "IDatabase.h"
#include "sqlite3.h"

class SqliteDataBase : public IDatabase
{
public:
	SqliteDataBase();
	~SqliteDataBase();

	bool open();
	bool close();

	// *** Queries ***
	int doesUserExist(const std::string& username);
	int doesPasswordMatch(const std::string& username, const std::string& password);
	
	// statistics Queries
	double getPlayerAverageAnswerTime(const std::string& username);
	int getNumOfCorrectAnswers(const std::string& username);
	int getNumOfTotalAnswers(const std::string& username);
	int getNumOfPlayerGames(const std::string& username);
	int getPlayerScore(const std::string& username);
	std::vector<std::string> getHighScores();
	int updatePlayerStats(PlayerResults playerResults, std::vector<std::string> userStatistics, int score);

	// Operations
	int addNewUser(const std::string& username, const std::string& password, const std::string& email);

	// Questions
	std::vector<Question> getQuestions(int amountOfQuestions);

private:
	int runStaticQueries(const std::string& query);
	
	template<typename... Args>
	QueryResult runDynamicQueries(const std::string& query, const Args&... args);
	QueryResult getDynamicResults(sqlite3_stmt* res);

	template<typename T, typename... Args>
	void bindAll(sqlite3_stmt* res, int index, const T& first, const Args&... args);
	void bindAll(sqlite3_stmt* res, int index);

	// attach for attacing string
	void attachSingleInput(sqlite3_stmt* res, int index, const std::string& first);
	
	// attach for attaching int
	void attachSingleInput(sqlite3_stmt* res, int index, int first);

	// attach for attaching double
	void attachSingleInput(sqlite3_stmt* res, int index, double first);

	// aattach for attaching float
	void attachSingleInput(sqlite3_stmt* res, int index, float first);

	int addInitialQuestions();

	Question buildQuestion(const QueryResult& result);

	sqlite3* db;
};