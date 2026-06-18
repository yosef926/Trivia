#pragma once

#include <vector>

#include "Communicator.h"
#include "Constants.h"
#include "IRequestHandler.h"

class JsonResponsePacketDeserializer
{
public:
	static RequestInfo buildRequestInfo(uint8_t id, const std::vector<uint8_t>& buffer);
	static requests::LoginRequest DeserializeLoginRequest(const std::vector<uint8_t>& buffer);
	static requests::SignupRequest DeserializeSignupRequest(const std::vector<uint8_t>& buffer);
	static requests::GetPlayersInRoomRequest deserializeGetPlayersRequest(const std::vector<uint8_t>& buffer);
	static requests::JoinRoomRequest deserializeJoinRoomRequest(const std::vector<uint8_t>& buffer);
	static requests::CreateRoomRequest deserializeCreateRoomRequest(const std::vector<uint8_t>& buffer);
	static requests::SubmitAnswerRequest deserializeSubmitAnswerRequest(const std::vector<uint8_t>& buffer);


private:
	JsonResponsePacketDeserializer() = default; // To prevent any creations of instances of this object
};