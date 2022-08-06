#include <iostream>
#include <memory>
#include <signal.h>

#include "server/iserver.hpp"
#include "server/tcpserver.hpp"

static constexpr const short port = 8088;
static std::unique_ptr<Server::IServer> server = std::unique_ptr<Server::IServer>(new Server::TcpServer());

void my_handler(int s)
{ 
    server->close();
}

int main(int argc, char **argv)
{
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    try
    {
        server->bind(port);
        server->work();
        server->close();
    }
    catch(const std::exception &ex)
    {
        std::cerr << "Throw: " << ex.what() << std::endl;
    }


    return 0;
}