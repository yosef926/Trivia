#include <stdexcept>

#include "Communicator.h"
#include "Server.h"
#include "Helper.h"
#include "LoginRequestHandler.h"
#include "JsonResponsePacketDeserializer.h"
#include "RequestHandlerFactory.h"
#include "RoomAdminRequestHandler.h"

Communicator::Communicator(RequestHandlerFactory& handlerFactory) 
	: m_handlerFactory(handlerFactory)
{
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_serverSocket == INVALID_SOCKET)
		throw std::runtime_error(std::string(__FUNCTION__) + " - Socket creation failed. Error: " + std::to_string(WSAGetLastError()));
	bindAndListen();
}


Communicator::~Communicator()
{
	try
	{
		closesocket(m_serverSocket);
		m_client.clear();
	}
	catch (...) {}
}


void Communicator::startHandleRequests()
{
	while (true)
	{
		std::cout << "Waiting for client connection request" << std::endl;
		// this accepts the client and create a specific socket from server to this client
		// the process will not continue until a client connects to the server
		SOCKET client_socket = accept(m_serverSocket, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
			throw std::runtime_error(__FUNCTION__);

		std::cout << "\nClient accepted. Server and client can speak" << std::endl;

		addClientToMap(client_socket);

		// the function that handle the conversation with the client
		std::thread clientThread(&Communicator::handleNewClient, this, client_socket);
		clientThread.detach();
	}
}


void Communicator::bindAndListen()
{
	struct sockaddr_in sa = {};

	sa.sin_port = htons(PORT); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (bind(m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		std::cout << "CRITICAL: Bind failed with error: " << error << std::endl;
		throw std::runtime_error("Bind has failed");
	}
	// Start listening for incoming requests of clients
	if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::runtime_error("Listen has failed");
	std::cout << "Listening on port " << PORT << std::endl;
}


void Communicator::handleNewClient(SOCKET client_socket)
{
	try
	{
		while (true)
		{
			dataHeaders headers = Helper::getMessageHeaders(client_socket);
			std::cout << "\n\n[GOT REQUEST, CODE]: " << (int)headers.code << std::endl;

			std::vector<uint8_t> messageContent = Helper::getMessageContent(client_socket, headers.dataLength);

			RequestInfo reqInfo = JsonResponsePacketDeserializer::buildRequestInfo(headers.code, messageContent);

			IRequestHandler* handler = getClient(client_socket);

			RequestResult reqResult = handler->handleRequest(reqInfo);

			updateRequestType(reqResult.newHandler, handler, client_socket);
			
			Helper::sendData(client_socket, reqResult.response);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		if (client_socket != INVALID_SOCKET)
		{
			closesocket(client_socket);
			m_client.erase(client_socket);
			client_socket = INVALID_SOCKET;
		}
	}
	std::cout << "SOCKET and THREAD is closed!" << std::endl;
}


IRequestHandler* Communicator::getClient(const SOCKET& client_socket)
{
	auto it = m_client.find(client_socket);

	if (it != m_client.end())
	{
		return it->second.get();
	}
	else throw std::runtime_error("client doesn't exists in map");
}


void Communicator::updateRequestType(IRequestHandler* new_request_handler, IRequestHandler* old_handler, const SOCKET& client_socket)
{
	if (new_request_handler != old_handler)
	{
		for (auto& client_pair : m_client)
		{
			if (client_pair.first == client_socket)
			{
				client_pair.second = std::unique_ptr<IRequestHandler>(new_request_handler);
				break;
			}
		}
	}
}

void Communicator::addClientToMap(const SOCKET& client_sock)
{
	LoginRequestHandler* req_handler = m_handlerFactory.createLoginRequestHandler();

	m_client.insert({client_sock, std::unique_ptr<IRequestHandler>(req_handler)});
}