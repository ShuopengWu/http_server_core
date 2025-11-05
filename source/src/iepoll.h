#ifndef IEPOLL_H
#define IEPOLL_H

#include <sys/epoll.h>
#include <vector>

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
    void add_fd(int fd, uint32_t event_type);
    void delete_fd(int fd);
    int poll(epoll_event *events, int max_event_size = MAX_EVENT_SIZE, int timeout = DEFAULT_TIMEOUT);
private:
    int epoll_fd;
};

#endif