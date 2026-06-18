#include "RoomMemberRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "MenuRequestHandler.h"
#include "LoginRequestHandler.h"

RoomMemberRequestHandler::RoomMemberRequestHandler(RequestHandlerFactory& handlerFactory, const LoggedUser loggedUser, uint8_t roomId, RoomManager& roomManager)
	: m_roomId(roomId),
      m_user(loggedUser),
      m_roomManager(roomManager),
      m_handlerFactory(handlerFactory),
	  m_handlers({
			{ static_cast<uint8_t>(RequestsCode::LEAVE_ROOM), [this](const RequestInfo&) { return this->leaveRoom(); } },
			{ static_cast<uint8_t>(RequestsCode::GET_ROOM_STATE), [this](const RequestInfo&) { return this->getRoomState(); } },
			{ static_cast<uint8_t>(RequestsCode::LOGOUT), [this](const RequestInfo&) { return this->logout(); } }
	  })
{
}

RoomMemberRequestHandler::~RoomMemberRequestHandler()
{

}


bool RoomMemberRequestHandler::isRequestRelevant(const RequestInfo& reqInfo)
{
	return reqInfo.id == static_cast<uint8_t>(RequestsCode::LEAVE_ROOM)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::GET_ROOM_STATE)
	    || reqInfo.id == static_cast<uint8_t>(RequestsCode::LOGOUT);
}


RequestResult RoomMemberRequestHandler::handleRequest(const RequestInfo& reqInfo)
{
	if (isRequestRelevant(reqInfo))
	{
		auto it = m_handlers.find(reqInfo.id);
		if (it != m_handlers.end())
		{
			return it->second(reqInfo);
		}
	}
	responses::ErrorResponse response = { ErrorStatus::ROOM_MEMEBER, NOT_RELEVANT_REQUEST_MESSAGE };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);
	RequestResult reqResult = { serialResponse, this };
	return reqResult;
}


RequestResult RoomMemberRequestHandler::leaveRoom()
{
	auto roomOpt = m_roomManager.getRoom(m_roomId);
	LeaveRoomStatus responseStatus = roomOpt.value().get().removeUser(m_user);
	responses::LeaveRoomResponse response = { responseStatus };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler;
	if (responseStatus == LeaveRoomStatus::SUCCESS)
	{
		reqHandler = m_handlerFactory.createMenuRequestHandler(m_user);
	}
	else
	{
		reqHandler = this;
	}

	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult RoomMemberRequestHandler::getRoomState()
{
	auto roomOpt = m_roomManager.getRoom(m_roomId);
	RoomStatus roomStatus;
	if (roomOpt)
	{
		roomStatus = roomOpt.value().get().getRoomData().status;
	}
	else
	{
		throw std::runtime_error("Room not found");
	}
	std::cout << "[ROOM STATUS]: " << static_cast<int>(roomStatus);
	responses::GetRoomStateResponse response = { GetRoomStateStatus::SUCCESS, roomStatus,  roomOpt.value().get().getAllUsers(),  roomOpt.value().get().getRoomData().numOfQuestionsInGame,  roomOpt.value().get().getRoomData().timePerQuestions};
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler;
	if (roomStatus == RoomStatus::CLOSE_ROOM)
	{
		reqHandler = m_handlerFactory.createMenuRequestHandler(m_user);
	}
	else if (roomStatus == RoomStatus::RUNNING)
	{
		reqHandler = m_handlerFactory.createGameRequestHandler(m_user, m_roomId);
	}
	else
	{
		reqHandler = this;
	}
	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult RoomMemberRequestHandler::logout()
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