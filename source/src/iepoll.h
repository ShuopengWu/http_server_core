#ifndef IEPOLL_H
#define IEPOLL_H

#include <sys/epoll.h>
#include <vector>

class Channel;

namespace
{
static constexpr int DEFAULT_TIMEOUT = -1;
static constexpr size_t MAX_EVENT_SIZE = 1000;
} // namespace

class IEpoll
{
public:
    IEpoll();
    ~IEpoll();
    void update_channel(Channel *channel);
    void delete_fd(int fd);
    int poll(std::vector<Channel *> &channels, size_t max_event_size = MAX_EVENT_SIZE, int timeout = DEFAULT_TIMEOUT);
private:
    int epoll_fd;
    std::vector<epoll_event> events;
};

#endif