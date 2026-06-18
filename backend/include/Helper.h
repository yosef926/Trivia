#pragma once

#include <WinSock2.h>
#include <string>
#include <random>
#include <cstdint>
#include <set>

#include "constants.h"

class Server;

struct dataHeaders {
	uint8_t code;
	uint32_t dataLength;
};


class Helper
{
public:
	static std::vector<uint8_t> getMessageContent(const SOCKET& sock, int bytesNum);
	static void sendData(const SOCKET& sc, const std::vector<uint8_t>& message);
	
	static void printVector(const std::vector<uint8_t>& data);
	
	static dataHeaders getMessageHeaders(const SOCKET& sock);
	
	static uint8_t generateId();
	static uint8_t generateUniqeIdInRange(std::set<uint8_t> alreadyBeenSet, uint8_t startRange, uint8_t endRange);

private:
};