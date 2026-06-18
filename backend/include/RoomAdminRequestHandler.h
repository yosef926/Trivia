#pragma once

#include <functional>

#include "IRequestHandler.h"
#include "RoomManager.h"
#include "RequestHandlerFactory.h"

class RequestHandlerFactory;

class RoomAdminRequestHandler : public IRequestHandler
{
public:
	typedef std::function<RequestResult(const RequestInfo&)> HandlerFunc;

	RoomAdminRequestHandler(RequestHandlerFactory& handlerFactory, const LoggedUser loggedUser, uint8_t roomId, RoomManager& roomManager);
	~RoomAdminRequestHandler();

	bool isRequestRelevant(const RequestInfo& reqInfo);
	RequestResult handleRequest(const RequestInfo& reqInfo);

private:
	RequestResult closeRoom();
	RequestResult startGame();
	RequestResult getRoomState();
	RequestResult logout();

	const LoggedUser m_user;
	uint8_t m_roomId;
	RoomManager& m_roomManager;
	RequestHandlerFactory& m_handlerFactory;

	// A dispatch table for all handler functions
	std::map<uint8_t, HandlerFunc> m_handlers;
};