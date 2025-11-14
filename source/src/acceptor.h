#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <memory>

#include "epollloop.h"
#include "channel.h"

class Acceptor
{
public:
    Acceptor(EpollLoop *epoll_loop, const std::string &ip = "", uint16_t port = 0);
    void set_accept_callback(const callback_t &callback);
    void set_accept_callback(callback_t &&callback);
    ~Acceptor();
private:
    std::unique_ptr<Channel> server_channel;
    callback_t accept_callback;
};

#endif