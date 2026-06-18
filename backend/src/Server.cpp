#include "Server.h"
#include "Helper.h"
#include "SqliteDataBase.h"
#include "IDatabase.h"

Server::Server()
	: m_database(new SqliteDataBase()),
	  m_handlerFactory(m_database),
	  m_communicator(m_handlerFactory)
{
}


Server::~Server()
{
	delete m_database;
}


void Server::run()
{
	std::thread t_connector(&Communicator::startHandleRequests, &m_communicator);
	t_connector.detach();
}