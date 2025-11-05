#include "iepoll.h"

#include "util.h"
#include "logger.h"

IEpoll::IEpoll()
{
    epoll_fd = epoll_create1(0);
    util::error_assert_with_errno(epoll_fd != -1, "Epoll create failed");
}

IEpoll::~IEpoll()
{
    util::close_fd(epoll_fd);
}

void IEpoll::add_fd(int fd, uint32_t event_type)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = event_type;
    int result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    util::error_assert_with_errno(result != -1, "Add epoll failed");
}

void IEpoll::delete_fd(int fd)
{
    int result = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);

    if (result < 0)
    {
        logger::logger::instance().show_waring_log("Epoll delete fd : " + std::to_string(fd) + " failed");
    }
}

int IEpoll::poll(epoll_event *events, int max_event_size, int timeout)
{
    if (events == nullptr || max_event_size == 0)
        return -1;

    int result = epoll_wait(epoll_fd, events, max_event_size, timeout);
    util::error_assert_with_errno(result != -1, "Find a error in epoll wait");
    return result;
}