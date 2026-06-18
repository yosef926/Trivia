#include "LoginRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonResponsePacketDeserializer.h"
#include "Helper.h"
#include "MenuRequestHandler.h"
#include "RequestHandlerFactory.h"

LoginRequestHandler::LoginRequestHandler(RequestHandlerFactory& HandlerFactory)
	: m_handlerFactory(HandlerFactory)
{
	this->m_handlers = {
	{ static_cast<uint8_t>(RequestsCode::LOGIN), &LoginRequestHandler::login },
	{ static_cast<uint8_t>(RequestsCode::SIGNUP), &LoginRequestHandler::signup }
	};
}


LoginRequestHandler::~LoginRequestHandler()
{
}

bool LoginRequestHandler::isRequestRelevant(const RequestInfo& reqInfo)
{
	return reqInfo.id == static_cast<uint8_t>(RequestsCode::LOGIN) || reqInfo.id == static_cast<uint8_t>(RequestsCode::SIGNUP);
}


RequestResult LoginRequestHandler::handleRequest(const RequestInfo& reqInfo)
{
	if (isRequestRelevant(reqInfo))
	{
		auto it = m_handlers.find(reqInfo.id);
		if (it != m_handlers.end())
		{
			return (this->*(it->second))(reqInfo);
		}
	}
	responses::ErrorResponse response = { ErrorStatus::LOGIN, NOT_RELEVANT_REQUEST_MESSAGE };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);
	RequestResult reqResult = { serialResponse, this };
	return reqResult;
}

RequestResult LoginRequestHandler::login(const RequestInfo& reqInfo)
{
	requests::LoginRequest loginReq = JsonResponsePacketDeserializer::DeserializeLoginRequest(reqInfo.buffer);

	LoginStatus responseStatus = m_handlerFactory.getLoginManager().login(loginReq.username, loginReq.password);
	responses::LoginResponse response = { responseStatus };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler;
	if (responseStatus == LoginStatus::SUCCESS)
	{
		LoggedUser loggedUser(loginReq.username);
		reqHandler = m_handlerFactory.createMenuRequestHandler(loggedUser);
	}
	else
	{
		reqHandler = this;
	}

	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}

RequestResult LoginRequestHandler::signup(const RequestInfo& reqInfo)
{
	requests::SignupRequest signupReq = JsonResponsePacketDeserializer::DeserializeSignupRequest(reqInfo.buffer);

	SignupStatus responseStatus = m_handlerFactory.getLoginManager().signup(signupReq.username, signupReq.password, signupReq.email);
	responses::SignupResponse response = { responseStatus };
	std::vector<uint8_t> serialResponse = JsonResponsePacketSerializer::serializeResponse(response);

	// Build handler
	IRequestHandler* reqHandler;
	if (responseStatus == SignupStatus::SUCCESS)
	{
		LoggedUser loggedUser(signupReq.username);
		reqHandler = m_handlerFactory.createMenuRequestHandler(loggedUser);
	}
	else
	{
		reqHandler = this;
	}
	
	RequestResult reqResult = { serialResponse, reqHandler };

	return reqResult;
}
