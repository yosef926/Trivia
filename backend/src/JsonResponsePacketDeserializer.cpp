#include <ctime>

#include "JsonResponsePacketDeserializer.h"




RequestInfo JsonResponsePacketDeserializer::buildRequestInfo(uint8_t id, const std::vector<uint8_t>& buffer)
{
	RequestInfo req = {};

	req.id = id;
	req.receivalTime = std::time(nullptr);
	req.buffer = std::vector<uint8_t>(buffer.begin(), buffer.end());

	return req;
}

requests::LoginRequest JsonResponsePacketDeserializer::DeserializeLoginRequest(const std::vector<uint8_t>& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());
	std::cout << "[MESSAGE FROM CLIENT]: " << jsonStr << std::endl;

	try {
		json j = json::parse(jsonStr);

		requests::LoginRequest loginReq = {j["username"], j["password"]};
		return loginReq;
	}
	catch (json::parse_error& e) {
		throw std::runtime_error(std::string("Parse error in login process: ") + e.what());
	}
}


requests::SignupRequest JsonResponsePacketDeserializer::DeserializeSignupRequest(const std::vector<uint8_t>& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());
	std::cout << "[MESSAGE FROM CLIENT]: " << jsonStr << std::endl;

	try {
		json j = json::parse(jsonStr);

		requests::SignupRequest signupReq = {j["username"], j["password"], j["email"]};
		return signupReq;
	}
	catch (json::parse_error& e) {
		throw std::runtime_error(std::string("Parse error in signup process: ") + e.what());
	}
}

requests::GetPlayersInRoomRequest JsonResponsePacketDeserializer::deserializeGetPlayersRequest(const std::vector<uint8_t>& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());
	std::cout << "[MESSAGE FROM CLIENT]: " << jsonStr << std::endl;

	try {
		json j = json::parse(jsonStr);

		requests::GetPlayersInRoomRequest getPlayersInRoomReq = { j["roomId"] };
		return getPlayersInRoomReq;
	}
	catch (json::parse_error& e) {
		throw std::runtime_error(std::string("Parse error in GetPlayersInRoom process: ") + e.what());
	}
}

requests::JoinRoomRequest JsonResponsePacketDeserializer::deserializeJoinRoomRequest(const std::vector<uint8_t>& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());
	std::cout << "[MESSAGE FROM CLIENT]: " << jsonStr << std::endl;

	try {
		json j = json::parse(jsonStr);

		requests::JoinRoomRequest joinRoomRequest = { j["roomId"] };
		return joinRoomRequest;
	}
	catch (json::parse_error& e) {
		throw std::runtime_error(std::string("Parse error in JoinRoom process: ") + e.what());
	}
}

requests::CreateRoomRequest JsonResponsePacketDeserializer::deserializeCreateRoomRequest(const std::vector<uint8_t>& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());
	std::cout << "[MESSAGE FROM CLIENT]: " << jsonStr << std::endl;

	try {
		json j = json::parse(jsonStr);

		requests::CreateRoomRequest createRoomRequest;

		createRoomRequest.roomName = j["roomName"].get<std::string>();
		createRoomRequest.maxUsers = j["maxUsers"].get<uint8_t>();
		createRoomRequest.questionsCount = j["questionsCount"].get<uint8_t>();
		createRoomRequest.answerTimeout = j["answerTimeout"].get<uint8_t>();

		return createRoomRequest;
	}
	catch (json::parse_error& e) {
		throw std::runtime_error(std::string("Parse error in CreateRoom process: ") + e.what());
	}
}

requests::SubmitAnswerRequest JsonResponsePacketDeserializer::deserializeSubmitAnswerRequest(const std::vector<uint8_t>& buffer)
{
	std::string jsonStr(buffer.begin(), buffer.end());
	std::cout << "[MESSAGE FROM CLIENT]: " << jsonStr << std::endl;

	try {
		json j = json::parse(jsonStr);

		requests::SubmitAnswerRequest submitAnswerRequest = { j["answerId"] };
		return submitAnswerRequest;
	}
	catch (json::parse_error& e) {
		throw std::runtime_error(std::string("Parse error in submitAnswer process: ") + e.what());
	}
}
