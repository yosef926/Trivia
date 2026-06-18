#include "RoomAdminRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "MenuRequestHandler.h"
#include "LoginRequestHandler.h"

RoomAdminRequestHandler::RoomAdminRequestHandler(RequestHandlerFactory& handlerFactory, const LoggedUser loggedUser, uint8_t roomId, RoomManager& roomManager)
	: m_user(loggedUser),
	  m_roomId(roomId),
	  m_roomManager(roomManager),
	  m_handlerFactory(handlerFactory),
	  m_handlers({
			{ static_cast<uint8_t>(RequestsCode::CLOSE_ROOM), [this](const RequestInfo&) { return this->closeRoom(); } },
			{ static_cast<uint8_t>(RequestsCode::START_GAME), [this](const RequestInfo&) { return this->startGame(); } },
			{ static_cast<uint8_t>(RequestsCode::GET_ROOM_STATE), [this](const RequestInfo&) { return this->getRoomState(); } }
	  })
{
}

RoomAdminRequestHandler::~RoomAdminRequestHandler()
{

}


bool RoomAdminRequestHandler::isRequestRelevant(const RequestInfo& reqInfo)
{
	return reqInfo.id == static_cast<uint8_t>(RequestsCode::CLOSE_ROOM)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::START_GAME)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::GET_ROOM_STATE)
		|| reqInfo.id == static_cast<uint8_t>(RequestsCode::LOGOUT);
}

RequestResult RoomAdminRequestHandler::handleRequest(const RequestInfo& reqInfo)
{
	if (isRequestRelevant(reqInfo))
	{
		auto it = m_handlers.find(reqInfo.id);
		if (it != m_handlers.end())
		{
			return it->second(reqInfo);
		}
	}
	responses::ErrorResponse response = { ErrorStatus::ROOM_ADMIN, NOT_RELEVANT_REQUEST_MESSAGE };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);
	RequestResult reqResult = { serialResponse, this };
	return reqResult;
}

RequestResult RoomAdminRequestHandler::closeRoom()
{
	auto roomOpt = m_roomManager.getRoom(m_roomId);
	if (roomOpt)
	{
		roomOpt.value().get().changeStatus(RoomStatus::CLOSE_ROOM);
	}

	responses::CloseRoomResponse response = { CloseRoomStatus::SUCCESS };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);
	IRequestHandler* reqHandler = m_handlerFactory.createMenuRequestHandler(m_user);
	RequestResult reqResult = { serialResponse, reqHandler };
	return reqResult;
}

RequestResult RoomAdminRequestHandler::startGame()
{
	auto roomOpt = m_roomManager.getRoom(m_roomId);
	if (roomOpt)
	{
		roomOpt.value().get().changeStatus(RoomStatus::RUNNING);
	}
	else
	{	
		throw std::runtime_error("Room not found");
	}

	responses::StartGameResponse response = { StartGameStatus::SUCCESS };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);
	
	Game game = m_handlerFactory.getGameManager().createGame(m_roomId);
	std::cout << "[GAME ID]: " << static_cast<int>(game.getGameId()) << std::endl;
	IRequestHandler* reqHandler = m_handlerFactory.createGameRequestHandler(m_user, game.getRoomId());
	
	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult RoomAdminRequestHandler::getRoomState()
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

	std::vector<std::string> players = roomOpt.value().get().getAllUsers();

	responses::GetRoomStateResponse response = {
		GetRoomStateStatus::SUCCESS,
		roomStatus,
		players,
		roomOpt.value().get().getRoomData().numOfQuestionsInGame,
		roomOpt.value().get().getRoomData().timePerQuestions
	};

	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);
	RequestResult reqResult = { serialResponse, this };
	return reqResult;
}

RequestResult RoomAdminRequestHandler::logout()
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