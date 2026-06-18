#pragma once

#include <vector>

#include "Communicator.h"
#include "Constants.h"
#include "IRequestHandler.h"

class JsonResponsePacketSerializer
{
public:
	static std::vector<uint8_t> serializeResponse(const responses::ErrorResponse& errorResponse);
	static std::vector<uint8_t> serializeResponse(const responses::LoginResponse& loginResponse);
	static std::vector<uint8_t> serializeResponse(const responses::SignupResponse& signupResponse);
	static std::vector<uint8_t> serializeResponse(const responses::LogoutResponse& logoutResponse);
	static std::vector<uint8_t> serializeResponse(const responses::GetRoomsResponse& getRoomResponse);
	static std::vector<uint8_t> serializeResponse(const responses::GetPlayersInRoomResponse& getPlayersInRoomResponse);
	static std::vector<uint8_t> serializeResponse(const responses::GetHighScoreResponse& getHighScoreResponse);
	static std::vector<uint8_t> serializeResponse(const responses::GetPersonalStatsResponse& getPersonalStatsResponse);
	static std::vector<uint8_t> serializeResponse(const responses::JoinRoomResponse& joinRoomResponse);
	static std::vector<uint8_t> serializeResponse(const responses::CreateRoomResponse& createRoomResponse);
	static std::vector<uint8_t> serializeResponse(const responses::CloseRoomResponse& closeRoomResponse);
	static std::vector<uint8_t> serializeResponse(const responses::StartGameResponse& startGameResponse);
	static std::vector<uint8_t> serializeResponse(const responses::GetRoomStateResponse& getRoomStateResponse);
	static std::vector<uint8_t> serializeResponse(const responses::LeaveRoomResponse& leaveRoomResponse);
	static std::vector<uint8_t> serializeResponse(const responses::LeaveGameResponse& leaveGameResponse);
	static std::vector<uint8_t> serializeResponse(const responses::GetQuestionResponse& getQuestionResponse);
	static std::vector<uint8_t> serializeResponse(const responses::SubmitAnswerResponse& submitAnswerResponse);
	static std::vector<uint8_t> serializeResponse(const responses::GetGameResultsResponse& getGameResultsResponse);


private:
	JsonResponsePacketSerializer() = default; // To prevent any creations of instances of this object
	
	template <typename T>
	static std::vector<uint8_t> buildBuffer(const T& res);
};