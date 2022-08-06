#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include "iserver.hpp"

#include <string>
#include <vector>
#include <mutex>
#include <utility>
#include <future>
#include <atomic>
#include <netinet/in.h>
#include <sys/socket.h>
#include <map>
#include <queue>

namespace Server
{
    class TcpServer final : public IServer
    {
        public:
            TcpServer();
            ~TcpServer();

            void bind(const short &port = 8088) override;
            void work() noexcept override;
            void close() override;

        private:
            void sendToOtherHosts(int socket, const std::string &message);
            void closeClientSocket(int socket);

        private:
            int _serverSocket;
            struct sockaddr_in _address;

        private:
            std::atomic_bool _endThread;

        private:
            std::mutex _socketsMutex;
            std::map<int, std::future<void>> _socketsMap;

        private:
            std::queue<int> _socketsToDelete;
            void disconnectedSockets() noexcept;
    };
}

#endif