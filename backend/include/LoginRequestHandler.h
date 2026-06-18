#pragma once

#include "IRequestHandler.h"
#include "MenuRequestHandler.h"

class RequestHandlerFactory;

class LoginRequestHandler : public IRequestHandler
{
public:
	typedef RequestResult(LoginRequestHandler::* HandlerFunc)(const RequestInfo&);

	LoginRequestHandler(RequestHandlerFactory& HandlerFactory);
	~LoginRequestHandler();

	bool isRequestRelevant(const RequestInfo& reqInfo);
	RequestResult handleRequest(const RequestInfo& reqInfo);

private:
	RequestResult login(const RequestInfo& reqInfo);
	RequestResult signup(const RequestInfo& reqInfo);

	RequestHandlerFactory& m_handlerFactory;

	// A dispatch table for all handler functions
	std::map<uint8_t, HandlerFunc> m_handlers;
};