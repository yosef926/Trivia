#pragma once

#include <thread>
#include <chrono>
#include <atomic>

#include "IRequestHandler.h"
#include "Game.h"
#include "GameManager.h"
#include "RoomManager.h"
#include "RequestHandlerFactory.h"
#include "MenuRequestHandler.h"
#include "LoginRequestHandler.h"

class RequestHandlerFactory;

class GameRequestHandler : public IRequestHandler
{
public:
	typedef RequestResult(GameRequestHandler::* HandlerFunc)(const RequestInfo&);

	GameRequestHandler(RequestHandlerFactory& HandlerFactory, const LoggedUser user, Game& game, GameManager& gameManager);
	~GameRequestHandler();

	bool isRequestRelevant(const RequestInfo& reqInfo);
	RequestResult handleRequest(const RequestInfo& reqInfo);

	RequestResult getQuestion(const RequestInfo& reqInfo);
	RequestResult submitAnswer(const RequestInfo& reqInfo);
	RequestResult getGameResults(const RequestInfo& reqInfo);
	RequestResult leaveGame(const RequestInfo& reqInfo);
	RequestResult logout(const RequestInfo& reqInfo);

private:
	// Clock
	std::thread m_timerThread;
	std::atomic<bool> m_keepCounting{ false };
	std::atomic<float> m_finalElapsedTime{ 0.0f };
	
	void runTimer();
	void startTimerThread();

	Game& m_game;
	const LoggedUser m_user;
	GameManager& m_gameManager;
	RequestHandlerFactory& m_handlerFactory;

	// A dispatch table for all handler functions
	std::map<uint8_t, HandlerFunc> m_handlers;
	

};