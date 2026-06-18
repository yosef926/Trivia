#include "JsonResponsePacketSerializer.h"


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::ErrorResponse& errorResponse)
{
	return buildBuffer(errorResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::LoginResponse& loginResponse)
{
	return buildBuffer(loginResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::SignupResponse& signupResponse)
{
	return buildBuffer(signupResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::LogoutResponse& logoutResponse)
{
	return buildBuffer(logoutResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::GetRoomsResponse& getRoomResponse)
{
	return buildBuffer(getRoomResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::GetPlayersInRoomResponse& getPlayersInRoomResponse)
{
	return buildBuffer(getPlayersInRoomResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::GetHighScoreResponse& getHighScoreResponse)
{
	return buildBuffer(getHighScoreResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::GetPersonalStatsResponse& getPersonalStatsResponse)
{
	return buildBuffer(getPersonalStatsResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::JoinRoomResponse& joinRoomResponse)
{
	return buildBuffer(joinRoomResponse);
}


std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::CreateRoomResponse& createRoomResponse)
{
	return buildBuffer(createRoomResponse);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::CloseRoomResponse& closeRoomResponse)
{
	return buildBuffer(closeRoomResponse);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::StartGameResponse& startGameResponse)
{
	return buildBuffer(startGameResponse);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::GetRoomStateResponse& getRoomStateResponse)
{
	return buildBuffer(getRoomStateResponse);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::LeaveRoomResponse& leaveRoomResponse)
{
	return buildBuffer(leaveRoomResponse);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::LeaveGameResponse& leaveGameResponse)
{
	return buildBuffer(leaveGameResponse);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::GetQuestionResponse& getQuestionResponse)
{
	return buildBuffer(getQuestionResponse);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::SubmitAnswerResponse& submitAnswerResponse)
{
	return buildBuffer(submitAnswerResponse);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const responses::GetGameResultsResponse& getGameResultsResponse)
{
	return buildBuffer(getGameResultsResponse);
}


template <typename T>
std::vector<uint8_t> JsonResponsePacketSerializer::buildBuffer(const T& res)
{
	std::vector<uint8_t> buffer;


	json json_response = res;
	std::string jsonPayload = json_response.dump();

	// Print server response
	std::cout << "[SERVER RESPONSE]: " << jsonPayload << std::endl;

	// Pushing the code
	buffer.push_back(static_cast<uint8_t>(res.status));

	// Pushing the length of the message
	uint32_t size = static_cast<uint32_t>(jsonPayload.size());
	buffer.push_back((size >> 24) & 0xFF);
	buffer.push_back((size >> 16) & 0xFF);
	buffer.push_back((size >> 8) & 0xFF);
	buffer.push_back(size & 0xFF);

	// Pushing the message itself
	buffer.insert(buffer.end(), jsonPayload.begin(),jsonPayload.end());

	return buffer;
}