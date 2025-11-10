#include "channel.h"

#include "iepoll.h"

Channel::Channel(IEpoll *epoll, int fd) :
    belong_epoll(epoll),
    socket(std::make_unique<ISocket>(fd)),
    is_in_epoll(false)
{
}

void Channel::enable_reading()
{
    events = EPOLLIN | EPOLLET;
    belong_epoll->update_channel(this);
}

void Channel::enable_writing()
{
    if (!(events & EPOLLOUT))
    {
        events |= EPOLLOUT;
        belong_epoll->update_channel(this);
    }
}

void Channel::disable_writing()
{
    if (events & EPOLLOUT)
    {
        events &= ~ EPOLLOUT;
        belong_epoll->update_channel(this);
    }
}

ISocket *Channel::get_socket()
{
    return socket.get();
}

int Channel::get_socket_fd() const
{
    return socket->get_socket_fd();
}

std::string &Channel::get_send_buffer()
{
    return send_buffer;
}

void Channel::append_send_buffer(const std::string &data)
{
    send_buffer.append(data);
}
