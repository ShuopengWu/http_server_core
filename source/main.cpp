#include <sys/socket.h>
#include <errno.h>
#include <map>
#include <memory>

#include "src/isocket.h"
#include "src/inetaddress.h"
#include "src/iepoll.h"
#include "src/channel.h"

static std::map<int, std::unique_ptr<Channel>> connection_map;

enum class echo_status
{
    ALL_ECHO,
    PART_ECHO,
    ERROR_ECHO
};

void close_handler(IEpoll *epoll, Channel *channel);
echo_status do_echo(Channel *channel, int received_bytes, char *received_buffer);
void connection_event_handler(IEpoll *epoll, Channel *channel)
{
    logger::logger &log = logger::logger::instance();
    std::pair<int, InetAddress> addr_pair;

    while (true)
    {
        addr_pair = channel->get_socket()->accept();
        if (addr_pair.first != -1)
        {
            log.show_info_log("new connection : " + addr_pair.second.to_string());
            std::unique_ptr<Channel> new_channel = std::make_unique<Channel>(epoll, addr_pair.first);
            new_channel->enable_reading();
            connection_map[addr_pair.first] = std::move(new_channel);
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK) // 接受完毕
        {
            return;
        }
        else // 发生错误
        {
            log.show_waring_log("Accept error");
            break;
        }
    }
}

void read_event_handler(IEpoll *epoll, Channel *channel)
{
    char received_buffer[1024];
    ssize_t received_bytes = -1;
    echo_status status;

    int socket_fd = channel->get_socket_fd();
    while (true)
    {
        received_bytes = ::recv(socket_fd, received_buffer, sizeof(received_buffer), 0);

        if (received_bytes > 0) // 收到数据，继续读取
        {
            logger::logger::instance().show_info_log("Received data, message: \"" + std::string(received_buffer, received_bytes) + "\" on FD: " + std::to_string(socket_fd));
            status = do_echo(channel, received_bytes, received_buffer);
  
            if (status == echo_status::ERROR_ECHO)
            {
                logger::logger::instance().show_waring_log("Write error on buffered data on FD: " + std::to_string(socket_fd));
                close_handler(epoll, channel);
                return;
            }

            continue;
        }
        else if (received_bytes == 0) // 客户端优雅关闭
        {
            close_handler(epoll, channel);
            return;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) // 本次读取完成，正常退出
                return;
            close_handler(epoll, channel);
            return;
        }
    }
}

void write_event_handler(IEpoll *epoll, Channel *channel)
{
    std::string send_buffer = channel->get_send_buffer();
    if (send_buffer.empty())
        return;

    int socket_fd = channel->get_socket_fd();

    while (!send_buffer.empty())
    {
        ssize_t send_bytes = ::send(socket_fd, send_buffer.data(), send_buffer.size(), 0);

        if (send_bytes > 0)
        {
            logger::logger::instance().show_debug_log("Sent " + std::to_string(send_bytes) + " bytes from buffer on FD: " + std::to_string(socket_fd));
            send_buffer.erase(0, send_bytes);
        }
        else if (send_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) // 缓存区满
            return;
        else // 发生错误
        {
            logger::logger::instance().show_waring_log("Write error on buffered data on FD: " + std::to_string(socket_fd));
            close_handler(epoll, channel);

            return;
        }
    }

    channel->disable_writing();
}

void close_handler(IEpoll *epoll, Channel *channel)
{
    int socket_fd = channel->get_socket_fd();
    epoll->delete_fd(socket_fd);
    if (connection_map.count(socket_fd))
        connection_map.erase(socket_fd);
    logger::logger::instance().show_info_log("Close FD: " + std::to_string(socket_fd));
}

echo_status do_echo(Channel *channel, int received_bytes, char *received_buffer)
{
    int socket_fd = channel->get_socket_fd();

    ssize_t send_bytes = ::send(socket_fd, received_buffer, received_bytes, 0);

    if (send_bytes == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            logger::logger::instance().show_debug_log("Send buffer full on FD: " + std::to_string(socket_fd) + ". All data buffered.");
            channel->append_send_buffer(std::string(received_buffer, received_bytes));
            channel->enable_writing();
            return echo_status::PART_ECHO;
        }
        return echo_status::ERROR_ECHO;
    }
    else if (send_bytes < received_bytes)
    {
        logger::logger::instance().show_debug_log("Partial send (" + std::to_string(send_bytes) + " bytes). Buffer full on FD: " + std::to_string(socket_fd));

        channel->append_send_buffer(std::string(received_buffer + send_bytes, received_bytes - send_bytes));
        channel->enable_writing();
        return echo_status::PART_ECHO;
    }
    logger::logger::instance().show_debug_log("Sent " + std::to_string(received_bytes) + " bytes from buffer on FD: " + std::to_string(socket_fd));
    return echo_status::ALL_ECHO;
}

int main()
{
    ISocket socket;
    socket.set_nonblocking();

    InetAddress addr;
    socket.bind(addr);

    socket.listen();
    std::unique_ptr<IEpoll> epoll = std::make_unique<IEpoll>();

    std::unique_ptr<Channel> server_channel = std::make_unique<Channel>(epoll.get(), socket.get_socket_fd());
    server_channel->enable_reading();

    std::vector<Channel *> channels(1000);
    int channel_numbers = -1;
    while (true)
    {
        channel_numbers = epoll->poll(channels, channels.size());
        if (channel_numbers <= 0)
            continue;
        else
        {
            for (int i = 0; i < channel_numbers; i++)
            {
                if (channels[i]->get_socket_fd() == socket.get_socket_fd()) // new connection
                {
                    connection_event_handler(epoll.get(), channels[i]);
                }
                else if (channels[i]->get_revents() & EPOLLIN) // read event
                {
                    read_event_handler(epoll.get(), channels[i]);
                }
                else if (channels[i]->get_revents() & EPOLLOUT) // write event
                {
                    write_event_handler(epoll.get(), channels[i]);
                }
                else
                {
                    if (channels[i]->get_revents() & (EPOLLERR | EPOLLHUP))
                        close_handler(epoll.get(), channels[i]);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
