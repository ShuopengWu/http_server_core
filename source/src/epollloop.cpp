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
}

void EpollLoop::start()
{
    is_stop = false;
    loop();
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
	    if (channels[i])
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
	int fd = channel->get_socket_fd();
        connection_map[fd] = std::move(channel);
        connection_map[fd]->set_is_in_epoll(true);
    }
}

void EpollLoop::remove_channel(Channel *channel)
{
    int key = channel->get_socket_fd();
    if (connection_map.find(key) != connection_map.end())
    {
	channel->set_is_in_epoll(false);
        epoll->delete_fd(key);
        connection_map.erase(key);
    }
}

Channel *EpollLoop::get_channel(int socket_fd)
{
    if (connection_map.find(socket_fd) != connection_map.end())
        return connection_map[socket_fd].get();
    
    return nullptr;
}
