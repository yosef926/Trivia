#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif
#include "WSAInitializer.h"
#include <iostream>
#include <exception>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <fstream>

#include "Helper.h"
#include "Server.h"


int main()
{
	try
	{
		WSAInitializer wsaInit;
		Server myServer;
		myServer.run();

		std::string adminInput;
		while (adminInput != "EXIT")
		{
			std::cin >> adminInput;
		}
		std::cout << "Bye bye..." << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	system("PAUSE");
	return 0;
}