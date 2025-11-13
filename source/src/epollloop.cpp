#include "epollloop.h"

#include "iepoll.h"
#include "channel.h"

EpollLoop::EpollLoop() :
    epoll(std::make_unique<IEpoll>()),
    is_stop(true)
{
}

EpollLoop::~EpollLoop()
{
    stop();
    loop();
}

void EpollLoop::start()
{
    is_stop = false;
}

void EpollLoop::stop()
{
    is_stop = true;
}

void EpollLoop::loop()
{
    std::vector<Channel *> channels;
    int result;
    while (!is_stop)
    {
        result = epoll->poll(channels);
        
        for (int i = 0; i < result; i++)
        {
            channels[i]->handle_event();
        }
    }
    
}

void EpollLoop::update_channel(Channel *channel)
{
    epoll->update_channel(channel);
}

void EpollLoop::add_channel(std::unique_ptr<Channel> channel)
{
    if (channel)
    {
        connection_map[channel->get_socket_fd()] = std::move(channel);
        channel->set_is_in_epoll(true);
    }
}

void EpollLoop::remove_channel(Channel *channel)
{
    int key = channel->get_socket_fd();
    if (connection_map.find(key) != connection_map.end())
    {
        epoll->delete_fd(key);
        connection_map.erase(key);
        channel->set_is_in_epoll(false);
    }
}

Channel *EpollLoop::get_channel(int socket_fd)
{
    if (connection_map.find(socket_fd) != connection_map.end())
        return connection_map[socket_fd].get();
    
    return nullptr;
}
