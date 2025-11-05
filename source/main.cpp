#include <sys/socket.h>
#include <errno.h>
#include <map>
#include <memory>

#include "src/isocket.h"
#include "src/inetaddress.h"
#include "iepoll.h"

static std::map<int, std::unique_ptr<ISocket>> connection_map;

void connection_event_handler(IEpoll &epoll, ISocket &server_socket)
{
    logger::logger &log = logger::logger::instance();
    std::pair<int, InetAddress> addr_pair;

    while (true)
    {
        addr_pair = server_socket.accept();
        if (addr_pair.first != -1)
        {
            std::unique_ptr<ISocket> client_socket = std::make_unique<ISocket>(addr_pair.first);
            client_socket->set_nonblocking();
            log.show_info_log("new connection : " + addr_pair.second.to_string());
            epoll.add_fd(client_socket->get_socket_fd(), EPOLLIN | EPOLLET);
            connection_map[addr_pair.first] = std::move(client_socket);
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)   // 接受完毕
        {
            return;
        }
        else                                                // 发生错误
        {
            logger::logger::instance().show_waring_log("Accept error");
            break;
        }
    }
}

void read_event_handler(IEpoll &epoll, int socket_fd)
{
    char received_buffer[1024];
    int received_bytes = -1;

    while (true)
    {
        received_bytes = ::recv(socket_fd, received_buffer, sizeof(received_buffer), 0);

        if (received_bytes > 0) // 收到数据，继续读取
        {
            logger::logger::instance().show_info_log("Recived data, message : """ + std::string(received_buffer, received_bytes) + """");
            
            // do echo
            ssize_t bytes_sent = ::send(socket_fd, received_buffer, received_bytes, 0);

            if (bytes_sent == -1)
            {
                break;
            }
            continue;
        }
        else if (received_bytes == 0) // 客户端优雅关闭
        {
            break;
        }
        else // 发生错误
        {
            break;
        }
    }
        logger::logger::instance().show_info_log("Connection on FD: " + std::to_string(socket_fd) + " closed.");
        epoll.delete_fd(socket_fd);
        if (connection_map.count(socket_fd))
            connection_map.erase(socket_fd);
}

int main()
{
    ISocket socket;
    socket.set_nonblocking();

    InetAddress addr;
    socket.bind(addr);

    socket.listen();
    IEpoll epoll;
    epoll.add_fd(socket.get_socket_fd(), EPOLLIN | EPOLLET);

    std::vector<epoll_event> events(1000);
    int event_numbers = -1;
    while (true)
    {
        event_numbers = epoll.poll(events.data(), events.size());
        if (event_numbers == -1)
        {
            logger::logger::instance().show_waring_log("Epoll poll may has a trouble");
            continue;
        }
        else if (event_numbers == 0)
            continue;
        else
        {
            for (int i = 0; i < event_numbers; i++)
            {
                if (events[i].data.fd == socket.get_socket_fd()) // new connection
                {
                    connection_event_handler(epoll, socket);
                }
                else if (events[i].events & EPOLLIN) // read event
                {
                    read_event_handler(epoll, events[i].data.fd);
                }
                else
                {
                    // TBD
                    continue;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
