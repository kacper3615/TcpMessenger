#ifndef ISERVER_HPP
#define ISERVER_HPP

namespace Server
{
    class IServer
    {
        public:
            IServer() = default;
            virtual ~IServer() = default;

            virtual void bind(const short &port = 8088) = 0;
            virtual void work() noexcept = 0;
            virtual void close() = 0;
    };
}

#endif