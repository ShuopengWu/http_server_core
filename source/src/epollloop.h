#ifndef EPOLLLOOP_H
#define EPOLLLOOP_H

#include <memory>
#include <map>
#include <vector>

class IEpoll;
class Channel;

class EpollLoop
{
public:
    EpollLoop();
    ~EpollLoop();
    void start();
    void stop();
    void loop();
    void update_channel(Channel *channel);

    void add_channel(std::unique_ptr<Channel> channel);
    void remove_channel(Channel *channel);
    Channel *get_channel(int socket_fd);
private:
    std::unique_ptr<IEpoll> epoll;
    bool is_stop;
    std::map<int, std::unique_ptr<Channel>> connection_map;
    std::vector<int> need_remove_fds;
};

#endif