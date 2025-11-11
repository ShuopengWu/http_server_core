#include "iepoll.h"

#include "util.h"
#include "logger.h"
#include "channel.h"

IEpoll::IEpoll()
{
    epoll_fd = epoll_create1(0);
    util::error_assert_with_errno(epoll_fd != -1, "Epoll create failed");
}

IEpoll::~IEpoll()
{
    util::close_fd(epoll_fd);
}

void IEpoll::update_channel(Channel *channel)
{
    int fd = channel->get_socket_fd();
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.data.ptr = channel;
    event.events = channel->get_events();

    if (channel->get_is_in_epoll())
    {
        int result = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
        util::error_assert_with_errno(result != -1, "Epoll ctl failed");
    }
    else
    {
        int result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
        util::error_assert_with_errno(result != -1, "Epoll ctl failed");
        channel->set_is_in_epoll(true);
    }
}

void IEpoll::delete_fd(int fd)
{
    int result = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);

    if (result < 0)
    {
        logger::logger::instance().show_warning_log("Epoll delete fd : " + std::to_string(fd) + " failed");
    }
}

int IEpoll::poll(std::vector<Channel *> &channels, size_t max_event_size, int timeout)
{
    if (max_event_size == 0)
        return 0;

    events.resize(max_event_size);

    channels.clear();

    int result = epoll_wait(epoll_fd, events.data(), max_event_size, timeout);

    util::error_assert_with_errno(result != -1, "Epoll wait find a error :", false);

    for (int i = 0; i < result; i++)
    {
        Channel* channel = static_cast<Channel *>(events[i].data.ptr);
        channel->set_revents(events[i].events);
        channels.push_back(channel);
    }

    return result;
}
