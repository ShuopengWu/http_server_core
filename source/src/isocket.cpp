#include "isocket.h"

#include "inetaddress.h"

ISocket::ISocket()
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    util::error_assert_with_errno(socket_fd != -1, "Create socket failed");
    set_socket_reusaddr();

    logger::logger::instance().show_debug_log("Socket created successfully. FD: " + std::to_string(socket_fd));
}

ISocket::~ISocket()
{
    if (socket_fd != -1)
    {
        close(socket_fd);

        logger::logger::instance().show_debug_log("Socket FD: " + std::to_string(socket_fd) + " closed successfully.");
    }
}

int ISocket::get_socket_fd() const
{
    return socket_fd;
}

void ISocket::bind(const InetAddress &inetaddr)
{
    sockaddr_in addr = inetaddr.get_addr();
    int result = ::bind(socket_fd, (sockaddr *)&addr, sizeof(addr));
    util::error_assert_with_errno(result != -1, "Socket bind failed");

    logger::logger::instance().show_debug_log("Socket FD: " + std::to_string(socket_fd) + " bound to address in " + inetaddr.to_string());
}


void ISocket::listen(int backlog)
{
    int result = ::listen(socket_fd, backlog);
    util::error_assert_with_errno(result != -1, "Socket listen failed");

    logger::logger::instance().show_debug_log("Socket FD: " + std::to_string(socket_fd) + " started listening with backlog " + std::to_string(backlog));
}

void ISocket::set_nonblocking()
{
    int now_fl_cntl = fcntl(socket_fd, F_GETFL);
    int result = fcntl(socket_fd, F_SETFL, now_fl_cntl | O_NONBLOCK);
    util::error_assert_with_errno(result != -1, "Failed to set socket to non-blocking state");

    logger::logger::instance().show_debug_log("Socket FD: " + std::to_string(socket_fd) + " set to non-blocking mode.");
}

std::pair<int, InetAddress> ISocket::accept()
{
    InetAddress inet_addr;
    sockaddr_in &addr = inet_addr.get_addr();
    socklen_t addr_len = sizeof(addr);
    int new_conn_fd = ::accept(socket_fd, (sockaddr *)&addr, &addr_len);

    if (new_conn_fd != -1) {
        logger::logger::instance().show_debug_log("Socket FD: " + std::to_string(socket_fd) + " accepted new connection. New FD: " + std::to_string(new_conn_fd));
    }
    return {new_conn_fd, inet_addr};
}

void ISocket::set_socket_reusaddr()
{
    int option_value = 1; // true
    int result = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value));
    if (result == -1)
        logger::logger::instance().show_waring_log("Failed to set socket port reuse");
    else
        logger::logger::instance().show_debug_log("Socket FD: " + std::to_string(socket_fd) + " set SO_REUSEADDR successfully.");
}
