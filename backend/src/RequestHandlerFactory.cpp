#include "RequestHandlerFactory.h"
#include "SqliteDataBase.h"
#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"


RequestHandlerFactory::RequestHandlerFactory(IDatabase* db)
	: m_database(db),
	  m_loginManager(db),
	  m_statisticsManager(db),
	  m_gameManager(db, m_roomManager)
{
}

RequestHandlerFactory::~RequestHandlerFactory()
{
	delete m_database;
}

LoginRequestHandler* RequestHandlerFactory::createLoginRequestHandler()
{
	LoginRequestHandler* reqHandler = new LoginRequestHandler(*this);
	return reqHandler;
}

MenuRequestHandler* RequestHandlerFactory::createMenuRequestHandler(const LoggedUser loggedUser)
{
	MenuRequestHandler* reqHandler = new MenuRequestHandler(*this, loggedUser);
	return reqHandler;
}

RoomAdminRequestHandler* RequestHandlerFactory::createRoomAdminRequestHandler(const LoggedUser loggedUser, uint8_t roomId)
{
	RoomAdminRequestHandler* reqHandler = new RoomAdminRequestHandler(*this, loggedUser, roomId, m_roomManager);
	return reqHandler;
}

RoomMemberRequestHandler* RequestHandlerFactory::createRoomMemberRequestHandler(const LoggedUser loggedUser, uint8_t roomId)
{
	RoomMemberRequestHandler* reqHandler = new RoomMemberRequestHandler(*this, loggedUser, roomId, m_roomManager);
	return reqHandler;
}

GameRequestHandler* RequestHandlerFactory::createGameRequestHandler(const LoggedUser loggedUser, uint8_t roomId)
{
	try
	{
		Game& liveGame = m_gameManager.getGame(roomId);
		return new GameRequestHandler(*this, loggedUser, liveGame, m_gameManager);
	}
	catch (const std::exception& e)
	{
		return nullptr;
	}
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
	return m_loginManager;
}

StatisticsManager& RequestHandlerFactory::getStatisticsManager()
{
	return m_statisticsManager;
}

RoomManager& RequestHandlerFactory::getRoomManager()
{
	return m_roomManager;
}

GameManager& RequestHandlerFactory::getGameManager()
{
	return m_gameManager;
}
