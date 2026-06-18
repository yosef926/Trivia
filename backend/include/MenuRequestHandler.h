#pragma once

#include <map>

#include "IRequestHandler.h"
#include "LoggedUser.h"

class RequestHandlerFactory;

class MenuRequestHandler : public IRequestHandler
{
public:
	typedef RequestResult(MenuRequestHandler::* HandlerFunc)(const RequestInfo&);

	MenuRequestHandler(RequestHandlerFactory& HandlerFactory, const LoggedUser user);
	~MenuRequestHandler();

	bool isRequestRelevant(const RequestInfo& reqInfo);
	RequestResult handleRequest(const RequestInfo& reqInfo);

private:
	RequestResult logout(const RequestInfo& reqInfo);
	RequestResult getRooms(const RequestInfo& reqInfo);
	
	RequestResult getPlayersInRoom(const RequestInfo& reqInfo);
	responses::GetPlayersInRoomResponse buildGetPlayersInRoomResponse(uint8_t roomId);

	RequestResult getPersonalStats(const RequestInfo& reqInfo);
	RequestResult getHighScore(const RequestInfo& reqInfo);

	RequestResult joinRoom(const RequestInfo& reqInfo);
	bool isRoomFull(Room& room);

	RequestResult createRoom(const RequestInfo& reqInfo);
	
	const LoggedUser m_user;
	RequestHandlerFactory& m_handlerFactory;

	// A dispatch table for all handler functions
	std::map<uint8_t, HandlerFunc> m_handlers;
};