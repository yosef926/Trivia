#pragma once

#include <vector>

#include "json.hpp"
#include "Constants.h"
#include "Room.h"

// Forward declerations
class IRequestHandler;
struct RoomData;
using json = nlohmann::json;


struct RequestInfo
{
	uint8_t id;
	time_t receivalTime;
	std::vector<uint8_t> buffer;
};

struct RequestResult
{
	std::vector<uint8_t> response;
	IRequestHandler* newHandler;
};

struct PlayerResults {
	std::string username;
	uint8_t correctAnswerCount;
	uint8_t wrongAnswerCount;
	double averageAnswerTime;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerResults, username, correctAnswerCount, wrongAnswerCount, averageAnswerTime);

namespace responses
{
	struct ErrorResponse {
		ErrorStatus status;
		std::string message;
	};

	struct LoginResponse {
		LoginStatus status;
	};

	struct SignupResponse {
		SignupStatus status;
	};
	
	struct LogoutResponse {
		LogoutStatus status;
	};

	struct GetRoomsResponse {
		GetRoomStatus status;
		std::vector<RoomData> rooms;
	};

	struct GetPlayersInRoomResponse {
		GetPlayersInRoomStatus status;
		std::vector<std::string> players;
	};

	struct GetHighScoreResponse {
		GetHighScoresStatus status;
		std::vector<std::string> statistics;
	};

	struct GetPersonalStatsResponse {
		GetPersonalStatsStatus status;
		std::vector<std::string> statistics;
	};

	struct JoinRoomResponse {
		JoinRoomStatus status;
	};

	struct CreateRoomResponse {
		CreateRoomStatus status;
	};

	struct CloseRoomResponse {
		CloseRoomStatus status;
	};

	struct StartGameResponse {
		StartGameStatus status;
	};

	struct GetRoomStateResponse {
		GetRoomStateStatus status;
		RoomStatus roomStatus;
		std::vector<std::string> players;
		uint8_t questionCount;
		uint8_t answerTimeout;
	};

	struct LeaveRoomResponse {
		LeaveRoomStatus status;
	};

	struct LeaveGameResponse {
		LeaveGameStatus status;
	};

	struct GetQuestionResponse {
		GetQuestionStatus status;
		std::string question;
		std::map<uint8_t, std::string> answers;
	};

	struct SubmitAnswerResponse {
		SubmitAnswerStatus status;
		uint8_t correctAnswerId;
	};

	struct GetGameResultsResponse {
		GetGameResultsStatus status;
		std::vector<PlayerResults> results;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ErrorResponse, message);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LoginResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SignupResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogoutResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetRoomsResponse, status, rooms);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetPlayersInRoomResponse, status, players);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetHighScoreResponse, status, statistics);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetPersonalStatsResponse, status, statistics);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JoinRoomResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateRoomResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CloseRoomResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StartGameResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetRoomStateResponse, status, roomStatus, players, questionCount, answerTimeout);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LeaveRoomResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LeaveGameResponse, status);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetQuestionResponse, status, question, answers);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SubmitAnswerResponse, status, correctAnswerId);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetGameResultsResponse, status, results);
}


namespace requests
{
	struct LoginRequest
	{
		std::string username;
		std::string password;
	};

	struct SignupRequest
	{
		std::string username;
		std::string password;
		std::string email;
	};
	
	struct GetPlayersInRoomRequest
	{
		uint8_t roomId;
	};

	struct JoinRoomRequest
	{
		uint8_t roomId;
	};

	struct CreateRoomRequest
	{
		std::string roomName;
		uint8_t maxUsers;
		uint8_t questionsCount;
		uint8_t answerTimeout;
	};

	struct SubmitAnswerRequest
	{
		uint8_t answerId;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LoginRequest, username, password);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SignupRequest, username, password, email);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetPlayersInRoomRequest, roomId);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JoinRoomRequest, roomId);
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateRoomRequest, roomName, maxUsers, questionsCount, answerTimeout);
};


class IRequestHandler
{
public:
	virtual ~IRequestHandler() = default;

	virtual bool isRequestRelevant(const RequestInfo& reqInfo) = 0;
	virtual RequestResult handleRequest(const RequestInfo& reqInfo) = 0;
private:

};	