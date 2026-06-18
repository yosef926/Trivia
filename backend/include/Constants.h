#pragma once

#include <cstdint>
#include <string>
#include <filesystem>

// MESSAGE FORMAT - (code-1byte)(length-4byte)()()()(....)
#define START_OF_DATA_OFFSET 5
#define DIFFERENCE_VALUE_ERROR_TO_REQUEST 9
#define AMOUNT_OF_QUESTION_IN_DB 10

enum class RequestsCode : uint8_t
{
    LOGIN = 100,
    SIGNUP = 110,
    LOGOUT = 120,
    GET_HIGH_SCORE = 130,
    GET_ROOM = 140,
    GET_PLAYERS_IN_ROOM = 150,
    JOIN_ROOM = 160,
    CREATE_ROOM = 170,
    GET_PERSONAL_STATS = 180,
    CLOSE_ROOM = 190,
    START_GAME = 200,
    GET_ROOM_STATE = 210,
    LEAVE_ROOM = 220,
    LEAVE_GAME = 230,
    GET_QUESTION = 240,
    SUBMIT_ANSWER = 245,
    GET_GAME_RESULTS = 250,
};

enum class ErrorStatus : uint8_t
{
    LOGIN = 109,
    MENU = 129,
    ROOM_ADMIN = 179,
    ROOM_MEMEBER = 149,
    GAME = 209
};

enum class LoginStatus : uint8_t
{
    SUCCESS = 101,
    USER_ALREADY_LOGGED = 102,
    INVALID_CREDENTIALS = 103
};

enum class SignupStatus : uint8_t
{
    SUCCESS = 111,
    USERNAME_ALREADY_EXIST = 112
};

enum class LogoutStatus : uint8_t
{
    SUCCESS = 121,
    FAIL = 122
};

enum class GetHighScoresStatus : uint8_t
{
    SUCCESS = 131,
    FAIL = 132
};

enum class GetRoomStatus : uint8_t
{
    SUCCESS = 141,
    FAIL = 142
};

enum class GetPlayersInRoomStatus : uint8_t
{
    SUCCESS = 151,
    FAIL = 152
};

enum class JoinRoomStatus : uint8_t
{
    SUCCESS = 161,
    FAIL = 162
};

enum class CreateRoomStatus : uint8_t
{
    SUCCESS = 171,
    FAIL = 172
};

enum class GetPersonalStatsStatus : uint8_t
{
    SUCCESS = 181,
    FAIL = 182
};

enum class CloseRoomStatus : uint8_t
{
    SUCCESS = 191,
    FAIL = 192
};

enum class StartGameStatus : uint8_t
{
    SUCCESS = 201,
    FAIL = 202
};

enum class GetRoomStateStatus : uint8_t
{
    SUCCESS = 211,
    FAIL = 212
};

enum class LeaveRoomStatus : uint8_t
{
    SUCCESS = 221,
    FAIL = 222
};

enum class LeaveGameStatus : uint8_t
{
    SUCCESS = 231,
    FAIL = 232
};

enum class GetQuestionStatus : uint8_t
{
    SUCCESS = 241,
    FAIL = 242
};

enum class SubmitAnswerStatus : uint8_t // Set those codes in the same 10 range of GetQuestionStatus, beacuse code of respnose is 1 byte(max 255).
{
    SUCCESS = 246,
    FAIL = 247
};

enum class GetGameResultsStatus : uint8_t
{
    GAME_STILL_RUNNING = 251,
    GAME_OVER = 252,
    FAIL = 253
};

enum class RoomStatus : uint8_t
{
    WAITING_FOR_PLAYERS = 30,
    RUNNING = 31,
    CLOSE_ROOM = 32
};


inline std::string NOT_RELEVANT_REQUEST_MESSAGE = "Request is not relevant for the current handler";
inline std::string GET_QUESTION_OUT_OF_RANGE_MESSAGE = "You finished All questions";
inline std::string SUBMIT_ANSWER_NOT_EXISTS = "The answer is not one of the options - error";

// *** DB related ***
inline std::filesystem::path dbPath = std::filesystem::current_path() / "backend" / "DB" / "trivia.db";
//inline std::string dbPath = "backend/DB/trivia.db"; 
inline std::filesystem::path QustionsJsonPath = std::filesystem::current_path() / "backend" / "DB" / "questions.json";
//inline std::string QustionsJsonPath = "DB/questions.json";

// Queries
inline std::string tablesInitializeQuery =
"CREATE TABLE IF NOT EXISTS t_users ("
"USERNAME TEXT PRIMARY KEY NOT NULL UNIQUE,"
"PASSWORD TEXT NOT NULL,"
"EMAIL TEXT NOT NULL);"

"CREATE TABLE IF NOT EXISTS t_statistics ("
"USERNAME TEXT PRIMARY KEY NOT NULL,"
"avgAnswerTime REAL,"
"numCorrectAnswers INTEGER,"
"numTotalAnswers INTEGER,"
"numGamesPlayed INTEGER,"
"score REAL,"
"FOREIGN KEY (USERNAME) REFERENCES t_users(USERNAME));"

"CREATE TABLE IF NOT EXISTS t_questions("
"question_ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
"question TEXT NOT NULL,"
"right_ans TEXT NOT NULL,"
"ans2 TEXT NOT NULL,"
"ans3 TEXT NOT NULL,"
"ans4 TEXT NOT NULL);";

// User table related
inline std::string ADD_USER_QUERY = "INSERT INTO t_users (USERNAME, PASSWORD, EMAIL) VALUES (?, ?, ?);";
inline std::string DOES_USER_EXIST_QUERY = "SELECT 1 FROM t_users WHERE USERNAME = ?;";
inline std::string DOES_PASSWORD_MATCH_QUERY = "SELECT 1 FROM t_users WHERE USERNAME = ? AND PASSWORD = ?;";

// Questions table related
inline std::string CHECK_QUESTIONS_EMPTY = "SELECT COUNT(*) FROM t_questions;";
inline std::string ADD_QUESTION_QUERY = "INSERT INTO t_questions (question, right_ans, ans2, ans3, ans4) VALUES (?, ?, ?, ?, ?);";
inline std::string GET_QUESTION = "SELECT * FROM t_questions WHERE question_ID = ?;";
inline std::string UPDATE_USER_STATS =
"INSERT OR REPLACE INTO t_statistics ("
"avgAnswerTime, "
"numCorrectAnswers, "
"numTotalAnswers, "
"numGamesPlayed, "
"score, "
"USERNAME"
") VALUES (?, ?, ?, ?, ?, ?);";

// Statistics related
inline std::string GET_PLAYER_AVG_ANSWER_TIME = "SELECT avgAnswerTime FROM t_statistics WHERE USERNAME = ?;";
inline std::string GET_NUM_CORRECT_ANSWERS = "SELECT numCorrectAnswers FROM t_statistics WHERE USERNAME = ?;";
inline std::string GET_NUM_TOTAL_ANSWERS = "SELECT numTotalAnswers FROM t_statistics WHERE USERNAME = ?;";
inline std::string GET_NUM_GAMES_PLAYED = "SELECT numGamesPlayed FROM t_statistics WHERE USERNAME = ?;";
inline std::string GET_PLAYER_SCORE = "SELECT score FROM t_statistics WHERE USERNAME = ?;";
inline std::string GET_TOP5_SCORES =
"SELECT u.username, s.score "
"FROM t_users u "
"JOIN t_statistics s ON u.username = s.username "
"ORDER BY s.score DESC "
"LIMIT 5;";