#pragma once

#include <WinSock2.h>
#include <map>
#include <string>
#include <memory>

class RequestHandlerFactory;
class IRequestHandler;

class Communicator
{
public:
	Communicator(RequestHandlerFactory& handlerFactory);
	~Communicator();

	void startHandleRequests();

private:
	void bindAndListen();
	void handleNewClient(SOCKET client_socket);

	void addClientToMap(const SOCKET& client_socket);
	IRequestHandler* getClient(const SOCKET& client_socket);
	void updateRequestType(IRequestHandler* new_request_handler, IRequestHandler* old_handler, const SOCKET& client_socket);

	std::map<SOCKET, std::unique_ptr<IRequestHandler>> m_client;
	SOCKET m_serverSocket;
	RequestHandlerFactory& m_handlerFactory;
};