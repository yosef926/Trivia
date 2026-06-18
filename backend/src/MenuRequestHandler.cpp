#include <iostream>

#include "MenuRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonResponsePacketDeserializer.h"
#include "LoginRequestHandler.h"
#include "Helper.h"
#include "StatisticsManager.h"

MenuRequestHandler::MenuRequestHandler(RequestHandlerFactory& HandlerFactory, const LoggedUser user)
	: m_user(user),
	  m_handlerFactory(HandlerFactory),
	  m_handlers({
			{ static_cast<uint8_t>(RequestsCode::LOGOUT), &MenuRequestHandler::logout },
			{ static_cast<uint8_t>(RequestsCode::GET_ROOM), &MenuRequestHandler::getRooms },
			{ static_cast<uint8_t>(RequestsCode::GET_PLAYERS_IN_ROOM), &MenuRequestHandler::getPlayersInRoom },
			{ static_cast<uint8_t>(RequestsCode::GET_PERSONAL_STATS), &MenuRequestHandler::getPersonalStats },
			{ static_cast<uint8_t>(RequestsCode::GET_HIGH_SCORE), &MenuRequestHandler::getHighScore },
			{ static_cast<uint8_t>(RequestsCode::JOIN_ROOM), &MenuRequestHandler::joinRoom },
			{ static_cast<uint8_t>(RequestsCode::CREATE_ROOM), &MenuRequestHandler::createRoom }
	  })
{
}

MenuRequestHandler::~MenuRequestHandler()
{
}

bool MenuRequestHandler::isRequestRelevant(const RequestInfo& reqInfo)
{
	return reqInfo.id == static_cast<uint8_t>(RequestsCode::CREATE_ROOM)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::GET_ROOM)
	    || reqInfo.id == static_cast<uint8_t>(RequestsCode::GET_PLAYERS_IN_ROOM)	
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::JOIN_ROOM)
	    || reqInfo.id == static_cast<uint8_t>(RequestsCode::GET_PERSONAL_STATS)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::GET_HIGH_SCORE)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::LOGOUT);
}

RequestResult MenuRequestHandler::handleRequest(const RequestInfo& reqInfo)
{
	if (isRequestRelevant(reqInfo))
	{
		auto it = m_handlers.find(reqInfo.id);
		if (it != m_handlers.end())
		{
			return (this->*(it->second))(reqInfo);
		}
	}
	responses::ErrorResponse response = { ErrorStatus::MENU, NOT_RELEVANT_REQUEST_MESSAGE };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);
	RequestResult reqResult = { serialResponse, this };
	return reqResult;
}

RequestResult MenuRequestHandler::logout(const RequestInfo& reqInfo)
{	
	LogoutStatus responseStatus = m_handlerFactory.getLoginManager().logout(m_user.getUsername());
	responses::LogoutResponse response = { responseStatus };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler;
	if (responseStatus == LogoutStatus::SUCCESS)
	{
		reqHandler = m_handlerFactory.createLoginRequestHandler();
	}
	else
	{
		reqHandler = this;
	}

	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult MenuRequestHandler::getRooms(const RequestInfo& reqInfo)
{
	RoomManager& roomManager = m_handlerFactory.getRoomManager();

	std::vector<RoomData> roomsData = roomManager.getRooms();

	responses::GetRoomsResponse response = { GetRoomStatus::SUCCESS, roomsData};
	std::vector<uint8_t> serialRespnose = JsonResponsePacketSerializer::serializeResponse(response);

	RequestResult reqResult = { serialRespnose, this };
	return reqResult;
}

RequestResult MenuRequestHandler::getPlayersInRoom(const RequestInfo& reqInfo)
{
	requests::GetPlayersInRoomRequest getPlayersInRoomRequest = JsonResponsePacketDeserializer::deserializeGetPlayersRequest(reqInfo.buffer);

	responses::GetPlayersInRoomResponse response = buildGetPlayersInRoomResponse(getPlayersInRoomRequest.roomId);
	
	std::vector<uint8_t> serialRespnose = JsonResponsePacketSerializer::serializeResponse(response);

	RequestResult reqResult = { serialRespnose, this };
	return reqResult;
}

RequestResult MenuRequestHandler::getPersonalStats(const RequestInfo& reqInfo)
{
	std::vector<std::string> userStatistics = m_handlerFactory.getStatisticsManager().getUserStatistics(m_user.getUsername());

	responses::GetPersonalStatsResponse response = { GetPersonalStatsStatus::SUCCESS, userStatistics};
	std::vector<uint8_t> serialRespnose = JsonResponsePacketSerializer::serializeResponse(response);

	RequestResult reqResult = { serialRespnose, this };
	return reqResult;
}

RequestResult MenuRequestHandler::getHighScore(const RequestInfo& reqInfo)
{
	std::vector<std::string> highScores = m_handlerFactory.getStatisticsManager().getHighScores();

	responses::GetHighScoreResponse response = { GetHighScoresStatus::SUCCESS, highScores};
	std::vector<uint8_t> serialRespnose = JsonResponsePacketSerializer::serializeResponse(response);

	RequestResult reqResult = { serialRespnose, this };
	return reqResult;
}

RequestResult MenuRequestHandler::joinRoom(const RequestInfo& reqInfo)
{
	requests::JoinRoomRequest joinRoomRequest = JsonResponsePacketDeserializer::deserializeJoinRoomRequest(reqInfo.buffer);
	auto room = m_handlerFactory.getRoomManager().getRoom(joinRoomRequest.roomId);

	JoinRoomStatus status = JoinRoomStatus::FAIL;
	if (room && isRoomFull(room.value().get()))
	{
		room.value().get().addUser(m_user);
		status = JoinRoomStatus::SUCCESS;
	}

	responses::JoinRoomResponse response = { status };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Switch to RoomMemberRequestHandler
	IRequestHandler* reqHandler = this;
	if (static_cast<int>(response.status) == static_cast<int>(JoinRoomStatus::SUCCESS))
	{
		reqHandler = m_handlerFactory.createRoomMemberRequestHandler(m_user, joinRoomRequest.roomId);
	}

	RequestResult reqResult = { serialResponse, reqHandler };
	return reqResult;
}

bool MenuRequestHandler::isRoomFull(Room& room)
{
	return room.getAllLoggedUsers().size() < room.getRoomData().maxPlayers;
}

RequestResult MenuRequestHandler::createRoom(const RequestInfo& reqInfo)
{
	requests::CreateRoomRequest createRoomRequest = JsonResponsePacketDeserializer::deserializeCreateRoomRequest(reqInfo.buffer);
	uint8_t id = m_handlerFactory.getRoomManager().findAvailableRoomId();
	RoomData roomData = { id, createRoomRequest.roomName, createRoomRequest.maxUsers, createRoomRequest.questionsCount, createRoomRequest.answerTimeout, RoomStatus::WAITING_FOR_PLAYERS };

	CreateRoomStatus status = m_handlerFactory.getRoomManager().createRoom(m_user, roomData);
	responses::CreateRoomResponse response = { status };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	IRequestHandler* reqHandler = m_handlerFactory.createRoomAdminRequestHandler(m_user, id);

	RequestResult reqResult = { serialResponse, reqHandler };
	return reqResult;
}

responses::GetPlayersInRoomResponse MenuRequestHandler::buildGetPlayersInRoomResponse(uint8_t roomId)
{
	GetPlayersInRoomStatus responseStatus;
	std::vector<std::string> playersVector;
	auto roomOpt = m_handlerFactory.getRoomManager().getRoom(roomId);
	if (roomOpt)
	{
		responseStatus = GetPlayersInRoomStatus::SUCCESS;
		playersVector = roomOpt.value().get().getAllUsers();
	}
	else
	{
		responseStatus = GetPlayersInRoomStatus::FAIL;
	}

	responses::GetPlayersInRoomResponse response = { responseStatus, playersVector };
	return response;
}