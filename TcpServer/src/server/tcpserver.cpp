#include "tcpserver.hpp"

#include <exception>
#include <unistd.h>
#include <iostream>

using namespace Server;

TcpServer::TcpServer()
{

}

TcpServer::~TcpServer() 
{

}

void TcpServer::bind(const short &port) 
{
    _endThread.exchange(false);

    if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
        throw std::runtime_error("socket");
  
    int opt = -1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
        throw std::runtime_error("setsockopt");

    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(port);
  
    if (::bind(_serverSocket, (struct sockaddr*)&_address, sizeof(_address)) < 0)
        throw std::runtime_error("bind");

    if (listen(_serverSocket, 3) < 0) 
        throw std::runtime_error("listen");    
}

void TcpServer::work() noexcept 
{
    const auto clientFunction = [=](int socket){
        constexpr const int bufferSize = 1024;
        static_assert(bufferSize > 0, "Buffer size must be higher than 0!!!");

        char buffer[bufferSize] = {0x0};
        while(true)
        {
            int result = read(socket, buffer, bufferSize);
            if(result > 0)
            {
                sendToOtherHosts(socket, buffer);
            }
            else
                break;
        }

        closeClientSocket(socket);
    };

    int addrlen = sizeof(_address);
    while(!_endThread.load())
    {
        int newSocket;
        if ((newSocket = accept(_serverSocket, (struct sockaddr*)&_address, (socklen_t*)&addrlen)) > 0) {

            std::cout << "Socket: " << newSocket << " connected" << std::endl;
            const std::lock_guard<std::mutex> guard(_socketsMutex);

            disconnectedSockets();

            _socketsMap[newSocket] = std::async(std::launch::async, clientFunction, newSocket);
        }
    }

    std::cout << "Closing server" << std::endl;
    shutdown(_serverSocket, SHUT_RDWR);
}

void TcpServer::close() 
{
    _endThread.exchange(true);
    disconnectedSockets();
}

void TcpServer::sendToOtherHosts(int socket, const std::string &message) 
{
    std::cout << "Socket: " << socket << " send: " << message << std::endl;

    const std::lock_guard<std::mutex> guard(_socketsMutex);

    disconnectedSockets();

    for(auto &_socket : _socketsMap)
    {
        if(_socket.first != socket)
            send(_socket.first, message.c_str(), message.size(), 0);    
    }
}

void TcpServer::closeClientSocket(int socket) 
{
    std::cout << "Socket: " << socket << " disconnected" << std::endl;
    const std::lock_guard<std::mutex> guard(_socketsMutex);
    _socketsToDelete.push(socket);
}

void TcpServer::disconnectedSockets() noexcept
{
    while(!_socketsToDelete.empty())   
    {
        auto socket = _socketsToDelete.front();

        if(_socketsMap.find(socket) != _socketsMap.end())        
        {        
            ::close(socket);
            _socketsMap.erase(socket);    
        }

        _socketsToDelete.pop();
    }
}