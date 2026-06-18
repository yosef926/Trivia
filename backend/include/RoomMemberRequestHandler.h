#pragma once

#include <functional>

#include "IRequestHandler.h"
#include "RoomManager.h"
#include "RequestHandlerFactory.h"

class RequestHandlerFactory;

class RoomMemberRequestHandler : public IRequestHandler
{
public:
	typedef std::function<RequestResult(const RequestInfo&)> HandlerFunc;

	RoomMemberRequestHandler(RequestHandlerFactory& handlerFactory, const LoggedUser loggedUser, uint8_t roomId, RoomManager& roomManager);
	~RoomMemberRequestHandler();

	bool isRequestRelevant(const RequestInfo& reqInfo);
	RequestResult handleRequest(const RequestInfo& reqInfo);

private:
	RequestResult leaveRoom();
	RequestResult getRoomState();
	RequestResult logout();

	uint8_t m_roomId;
	const LoggedUser m_user;
	RoomManager& m_roomManager;
	RequestHandlerFactory& m_handlerFactory;

	// A dispatch table for all handler functions
	std::map<uint8_t, HandlerFunc> m_handlers;
};