#include "acceptor.h"

#include "inetaddress.h"

Acceptor::Acceptor(EpollLoop *epoll_loop, const std::string &ip, uint16_t port)
{
    std::unique_ptr<ISocket> server_socket = std::make_unique<ISocket>();
    server_socket->set_nonblocking();
    ip.empty() || port == 0 ? server_socket->bind(InetAddress()) : server_socket->bind(InetAddress(ip, port));
    server_socket->listen();
    server_channel = std::make_unique<Channel>(epoll_loop, std::move(server_socket));    
    server_channel->enable_reading();
}

void Acceptor::set_accept_callback(const callback_t &callback)
{
    accept_callback = callback;
    server_channel->set_read_callback([this](Channel *channel) {
        if (this->accept_callback)
        {
            this->accept_callback(channel);
        }
    });
}

void Acceptor::set_accept_callback(callback_t &&callback)
{
    accept_callback = std::move(callback);
    server_channel->set_read_callback([this](Channel *channel) {
        if (this->accept_callback)
        {
            this->accept_callback(channel);
        }
    });
}

Acceptor::~Acceptor()
{

}
