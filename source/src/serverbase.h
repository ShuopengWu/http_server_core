#ifndef SERVERBASE_H
#define SERVERBASE_H

#include <memory>
#include <functional>

#include "epollloop.h"
#include "isocket.h"
#include "inetaddress.h"
#include "channel.h"
#include "acceptor.h"

class ServerBase
{
public:
    ServerBase();
    virtual ~ServerBase();
    void start();
    void stop();
protected:
    enum class read_callback_result
    {
        NONE,
        DO_WRITE,
        DO_CLOSE
    };
protected:
    virtual void on_connection(Channel *channel) = 0;
    virtual read_callback_result on_read(Channel *channel) = 0;
    virtual void on_write(Channel *channel) = 0;
    virtual void on_colse(Channel *channel) = 0;
private:
    void init_acceptor();
    void connection_event_callback(Channel *channel);
    void read_event_callback(Channel *channel);
    void write_event_callback(Channel *channel);
    void close_event_callback(Channel *channel);
    std::unique_ptr<EpollLoop> epoll_loop; 
    std::unique_ptr<Acceptor> acceptor;
};

#endif