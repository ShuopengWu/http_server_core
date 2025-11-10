#ifndef CHANNEL_H
#define CHANNEL_H

#include <cstdint>
#include <cstdlib>

#include "util.h"
#include "isocket.h"

class IEpoll;

class Channel
{
DECLARE_PROPERTY(std::uint32_t, events)
DECLARE_PROPERTY(std::uint32_t, revents)
DECLARE_PROPERTY(bool, is_in_epoll)
public:
    Channel(IEpoll *epoll, int fd);
    ISocket *get_socket();
    int get_socket_fd() const;
    std::string &get_send_buffer();
    void append_send_buffer(const std::string &data);

    void enable_reading();
    void enable_writing();
    void disable_writing();
private:
    IEpoll *belong_epoll;
    std::unique_ptr<ISocket> socket;
    std::string send_buffer;
};

#endif