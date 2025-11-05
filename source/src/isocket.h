#ifndef ISOCKET_H
#define ISOCKET_H

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>  //file control 文件控制

#include "util.h"

class InetAddress;

namespace
{
static constexpr int DEFAULT_BACKLOG = 0x0400;
}


class ISocket
{
public:
    ISocket();
    ISocket(int fd);
    ~ISocket();
    int get_socket_fd() const;
    void bind(const InetAddress &inetaddr);
    void listen(int backlog = DEFAULT_BACKLOG);
    void set_nonblocking();
    std::pair<int, InetAddress> accept();
private:
    void set_socket_reusaddr();
    int socket_fd;
};

#endif