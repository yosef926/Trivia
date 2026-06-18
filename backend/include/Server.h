#pragma once

#include <thread>

#include "RequestHandlerFactory.h"
#include "Communicator.h"

class IDatabase;


#define PORT 22861

class Server
{
public:
	Server();
	~Server();

	void run();

private:
	IDatabase* m_database;
	RequestHandlerFactory m_handlerFactory;
	Communicator m_communicator;
};