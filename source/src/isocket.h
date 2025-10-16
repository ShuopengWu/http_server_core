#ifndef ISOCKET_H
#define ISOCKET_H

// #include <sys/socket.h>

class ISocket
{
public:
    ISocket();
    ~ISocket();
    int get_socket_fd() const;
private:
    int socket_fd;
};

#endif