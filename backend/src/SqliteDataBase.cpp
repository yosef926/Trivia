#include <vector>
#include <filesystem>
#include <fstream>
#include <set>

#include "IDatabase.h"
#include "SqliteDataBase.h"
#include "sqlite3.h"
#include "Constants.h"
#include "json.hpp"
#include "IRequestHandler.h"
#include "Helper.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

SqliteDataBase::SqliteDataBase()
{
	open();
}


SqliteDataBase::~SqliteDataBase()
{
}


bool SqliteDataBase::open()
{
	fs::path nativePath = dbPath;
	bool alreadyExisted = fs::exists(nativePath);
	int rc = sqlite3_open(nativePath.string().c_str(), &db);

	if (!alreadyExisted && rc == SQLITE_OK)
	{
		std::cout << "DB has created!" << std::endl;
		runStaticQueries(tablesInitializeQuery);
		std::cout << "Tables has initialized!" << std::endl;
		addInitialQuestions();
		return 0; // Success
	}
	return 1; // Fail
}


bool SqliteDataBase::close()
{
	if (sqlite3_close(db) == SQLITE_OK) return true;
	return false;
}


int SqliteDataBase::addNewUser(const std::string& username, const std::string& password, const std::string& email)
{
	runDynamicQueries(ADD_USER_QUERY, username, password, email);
	return 0;
}

std::vector<Question> SqliteDataBase::getQuestions(int amountOfQuestions)
{
	std::vector<Question> questions;
	std::set<uint8_t> idOfQuestion;

	for (int i = 0; i < amountOfQuestions; i++)
	{
		uint8_t uniqeId = Helper::generateUniqeIdInRange(idOfQuestion, 1, 10);
		idOfQuestion.insert(uniqeId);

		QueryResult result = runDynamicQueries(GET_QUESTION, uniqeId);
		Question newQuestion = buildQuestion(result);
		questions.push_back(newQuestion);
	}

	return questions;
}


Question SqliteDataBase::buildQuestion(const QueryResult& result)
{
	Row singleRow = result.at(0);

	std::string questionText = std::get<std::string>(singleRow.at(1));
	std::string rightAns = std::get<std::string>(singleRow.at(2));
	std::string answer2 = std::get<std::string>(singleRow.at(3));
	std::string answer3 = std::get<std::string>(singleRow.at(4));
	std::string answer4 = std::get<std::string>(singleRow.at(5));

	std::vector<std::string> possibleAnswers = { rightAns, answer2, answer3, answer4 };
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(possibleAnswers.begin(), possibleAnswers.end(), g);

	auto it = std::find(possibleAnswers.begin(), possibleAnswers.end(), rightAns);
	int correctIndex = static_cast<int>(std::distance(possibleAnswers.begin(), it));

	Question newQuestion(questionText, possibleAnswers, correctIndex);
	return newQuestion;
}


int SqliteDataBase::addInitialQuestions()
{
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, CHECK_QUESTIONS_EMPTY.c_str(), -1, &stmt, nullptr);
	sqlite3_step(stmt);

	int count = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);

	//check if table is empty
	if (count > 0)
	{
		return 0;
	}

	std::ifstream file(QustionsJsonPath);
	if (!file.is_open())
	{
		throw std::runtime_error("Could not open questions.json");
	}

	json j;
	file >> j;

	//init each question from the json file
	for (auto& q : j["results"])
	{
		std::string question = q["question"];
		std::string correctAns = q["correct_answer"];
		std::string wrongAns1 = q["incorrect_answers"][0];
		std::string wrongAns2 = q["incorrect_answers"][1];
		std::string wrongAns3 = q["incorrect_answers"][2];

		runDynamicQueries(ADD_QUESTION_QUERY, question, correctAns, wrongAns1, wrongAns2, wrongAns3);
	}

	std::cout << "Questions loaded from JSON!" << std::endl;
	return 0;
}


int SqliteDataBase::doesUserExist(const std::string& username)
{
	return !runDynamicQueries(DOES_USER_EXIST_QUERY, username).at(0).empty();
}


int SqliteDataBase::doesPasswordMatch(const std::string& username, const std::string& password)
{
	return !runDynamicQueries(DOES_PASSWORD_MATCH_QUERY, username, password).at(0).empty();
}

double SqliteDataBase::getPlayerAverageAnswerTime(const std::string& username)
{
	QueryResult result = runDynamicQueries(GET_PLAYER_AVG_ANSWER_TIME, username);
	if (result.empty() || result.at(0).empty()) return 0.0;
	return std::get<double>(result.at(0).at(0));
}

int SqliteDataBase::getNumOfCorrectAnswers(const std::string& username)
{
	QueryResult result = runDynamicQueries(GET_NUM_CORRECT_ANSWERS, username);
	if (result.empty() || result.at(0).empty()) return 0;
	return std::get<int>(result.at(0).at(0));
}

int SqliteDataBase::getNumOfTotalAnswers(const std::string& username)
{
	QueryResult result = runDynamicQueries(GET_NUM_TOTAL_ANSWERS, username);
	if (result.empty() || result.at(0).empty()) return 0;
	return std::get<int>(result.at(0).at(0));
}

int SqliteDataBase::getNumOfPlayerGames(const std::string& username)
{
	QueryResult result = runDynamicQueries(GET_NUM_GAMES_PLAYED, username);
	if (result.empty() || result.at(0).empty()) return 0;
	return std::get<int>(result.at(0).at(0));
}

int SqliteDataBase::getPlayerScore(const std::string& username)
{
	QueryResult result = runDynamicQueries(GET_PLAYER_SCORE, username);
	if (result.empty() || result.at(0).empty()) return 0;
	return static_cast<int>(std::get<double>(result.at(0).at(0)));
}

std::vector<std::string> SqliteDataBase::getHighScores()
{
	std::vector<std::string> scores;
	QueryResult table = runDynamicQueries(GET_TOP5_SCORES);

	if (table.empty() || table.at(0).size() < 2) { return scores; }

	for (size_t i = 0; i < table.size(); i++)
	{
		if (table.at(i).size() < 2) { continue; }

		std::string username = std::get<std::string>(table.at(i).at(0));

		int scoreVal = 0;
		auto& scoreVariant = table.at(i).at(1);

		if (std::holds_alternative<double>(scoreVariant)) {
			scoreVal = static_cast<int>(std::get<double>(scoreVariant));
		}
		else if (std::holds_alternative<int>(scoreVariant)) {
			scoreVal = std::get<int>(scoreVariant);
		}

		scores.push_back(username + ": " + std::to_string(scoreVal));
	}

	return scores;
}
int SqliteDataBase::updatePlayerStats(PlayerResults playerResults, std::vector<std::string> userStatistics, int score)
{
	int numCorrectAnswers = std::stoi(userStatistics.at(1)) + playerResults.correctAnswerCount;
	int numTotalAnswers = std::stoi(userStatistics.at(2)) + playerResults.correctAnswerCount + playerResults.wrongAnswerCount;
	int numGamesPlayed = std::stoi(userStatistics.at(3)) + 1;


	int oldTotalAnswers = std::stoi(userStatistics.at(2));
	double oldAvgTime = std::stod(userStatistics.at(0));
	int totalNewAnswers = playerResults.correctAnswerCount + playerResults.wrongAnswerCount;

	double avgAnswerTime = oldAvgTime;
	if (numTotalAnswers > 0)
	{
		avgAnswerTime = ((oldAvgTime * oldTotalAnswers) + (playerResults.averageAnswerTime * totalNewAnswers)) / numTotalAnswers;
	}

	double oldAvgScore = std::stod(userStatistics.at(4));
	int oldGamesPlayed = std::stoi(userStatistics.at(3));
	double newScore = oldAvgScore;
	if (numGamesPlayed > 0)
	{
		newScore = ((oldAvgScore * oldGamesPlayed) + score) / numGamesPlayed;
	}

	QueryResult queryResult = runDynamicQueries(UPDATE_USER_STATS, avgAnswerTime, numCorrectAnswers, numTotalAnswers, numGamesPlayed, newScore, playerResults.username);
	return 0;
}


int SqliteDataBase::runStaticQueries(const std::string& query)
{
	// This function is in charge of pushing static data(like creating table)
	char* errMsg = nullptr;
	int rc = sqlite3_exec(db, tablesInitializeQuery.c_str(), nullptr, 0, &errMsg);

	if (rc != SQLITE_OK)
	{
		sqlite3_free(errMsg);
		throw std::runtime_error("Query failed");
	}
	return rc;
}


template<typename... Args>
QueryResult SqliteDataBase::runDynamicQueries(const std::string& query, const Args&... args)
{
	// This function pushes dynamic query, which means with user-input data evolved,
	// so to prevent problems such as sql injection, I used sqlite3_prepare_v2 that seperate the code and the data parts,
	// Which makes the process of comiple, build, run --> much safer than regualr sqlite3_exec with callbacks
	sqlite3_stmt* res;

	// Compile the SQL query
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &res, nullptr) != SQLITE_OK) {
		throw std::runtime_error(std::string("Failed to prepare statement: ") + sqlite3_errmsg(db));
	}

	bindAll(res, 1, args...);

	// Run the command
	return getDynamicResults(res);
}


QueryResult SqliteDataBase::getDynamicResults(sqlite3_stmt* res)
{
	QueryResult table;
	int stepResult;

	do
	{
		stepResult = sqlite3_step(res);

		if (stepResult != SQLITE_DONE && stepResult != SQLITE_ROW) {
			throw std::runtime_error(std::string("Query failed: ") + sqlite3_errmsg(db));
		}

		if (stepResult == SQLITE_DONE && table.empty()) {
		}

		Row row;
		int cols = sqlite3_column_count(res);

		if (stepResult == SQLITE_ROW) {
		}

		for (int i = 0; i < cols; i++) {
			int type = sqlite3_column_type(res, i);

			if (type == SQLITE_INTEGER) {
				int val = sqlite3_column_int(res, i);
				row.push_back(val);
			}
			else if (type == SQLITE_FLOAT) {
				double val = sqlite3_column_double(res, i);
				row.push_back(val);
			}
			else if (type == SQLITE_TEXT) {
				const char* text = reinterpret_cast<const char*>(sqlite3_column_text(res, i));
				std::string str(text ? text : "");
				row.push_back(std::string(text ? text : ""));
			}
		}
		table.push_back(row);
	} while (stepResult == SQLITE_ROW);

	sqlite3_finalize(res); // Clean memory
	return table;
}


template<typename T, typename... Args>
void SqliteDataBase::bindAll(sqlite3_stmt* res, int index, const T& first, const Args&... rest)
{
	attachSingleInput(res, index, first);
	bindAll(res, index + 1, rest...);
}


void SqliteDataBase::bindAll(sqlite3_stmt* res, int index)
{
	// Nothing - to stop recursion
}


void SqliteDataBase::attachSingleInput(sqlite3_stmt* res, int index, const std::string& first)
{
	int code = sqlite3_bind_text(res, index, first.c_str(), -1, SQLITE_TRANSIENT);
	if (code != SQLITE_OK) {
		std::string err = sqlite3_errmsg(db);
		sqlite3_finalize(res);
		throw std::runtime_error("Binding failed: " + err);
	}
}

void SqliteDataBase::attachSingleInput(sqlite3_stmt* res, int index, int first)
{
	int code = sqlite3_bind_int(res, index, first);
	if (code != SQLITE_OK) {
		std::string err = sqlite3_errmsg(db);
		sqlite3_finalize(res);
		throw std::runtime_error("Binding failed: " + err);
	}
}

void SqliteDataBase::attachSingleInput(sqlite3_stmt* res, int index, double first)
{
	int code = sqlite3_bind_double(res, index, first);
	if (code != SQLITE_OK) {
		std::string err = sqlite3_errmsg(db);
		sqlite3_finalize(res);
		throw std::runtime_error("Binding failed: " + err);
	}
}

void SqliteDataBase::attachSingleInput(sqlite3_stmt* res, int index, float first)
{
	int code = sqlite3_bind_double(res, index, static_cast<double>(first));
	if (code != SQLITE_OK) {
		std::string err = sqlite3_errmsg(db);
		sqlite3_finalize(res);
		throw std::runtime_error("Binding failed: " + err);
	}
}