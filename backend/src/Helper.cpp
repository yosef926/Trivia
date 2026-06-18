#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstring>
#include <stdexcept>

#include "Helper.h"

std::vector<uint8_t> Helper::getMessageContent(const SOCKET& sock, int bytesNum)
{
    if (bytesNum == 0)
    {
        std::vector<uint8_t> emptyBuffer;
        return emptyBuffer;
    }
    std::vector<uint8_t> buffer(bytesNum);
    int res = recv(sock, reinterpret_cast<char*>(buffer.data()), bytesNum, 0);
    if (res == 0) {
        throw std::runtime_error("connection closed");
    }
    if (res == SOCKET_ERROR) {
        throw std::runtime_error("couldn't recv data");
    }

    return buffer;
}


void Helper::sendData(const SOCKET& sock, const std::vector<uint8_t>& message)
{
    if (send(sock, reinterpret_cast<const char*>(message.data()), static_cast<int>(message.size()), 0) == SOCKET_ERROR)
    {
        throw std::runtime_error("sending message to client has failed");
    }
}

dataHeaders Helper::getMessageHeaders(const SOCKET& sock)
{
    dataHeaders headers = {};
    std::vector<uint8_t> headersContent = getMessageContent(sock, START_OF_DATA_OFFSET);
    
    if (headersContent.size() < START_OF_DATA_OFFSET)
    {
        throw std::runtime_error("Incomplete or invalid message headers received.");
    }
    
    uint32_t rawLength = 0;
    std::memcpy(&rawLength, &headersContent[1], sizeof(dataHeaders::dataLength));

    // ntohl assume data came as Big endian, then replace it to match the protcol my CPU works in(Big/Small Endian)
    headers.dataLength = ntohl(rawLength);
    headers.code = headersContent[0];

    return headers;
}

void Helper::printVector(const std::vector<uint8_t>& data)
{
    for (size_t i = 0; i < data.size(); i++)
    {
        std::cout << data.at(i);
    }
    std::cout << std::endl;
}

uint8_t Helper::generateId()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, 255);
    return static_cast<uint8_t>(distrib(gen));
}

uint8_t Helper::generateUniqeIdInRange(std::set<uint8_t> alreadyBeenSet, uint8_t startRange, uint8_t endRange)
{
    uint8_t id;
    do
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(startRange, endRange);
        id = static_cast<uint8_t>(distrib(gen));
    } while (alreadyBeenSet.count(id));
    return id;
}