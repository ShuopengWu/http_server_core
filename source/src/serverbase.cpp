#include "serverbase.h"

ServerBase::ServerBase() : epoll_loop(std::make_unique<EpollLoop>())
{
}

ServerBase::~ServerBase()
{
    epoll_loop->stop();
}

void ServerBase::start()
{
    if (server_channel == nullptr)
        init_server_channel();
    epoll_loop->start();
}

void ServerBase::stop()
{
    epoll_loop->stop();
}

void ServerBase::init_server_channel()
{
    std::unique_ptr<ISocket> server_socket = std::make_unique<ISocket>();
    server_socket->set_nonblocking();
    server_socket->bind(InetAddress());
    server_socket->listen();
    server_channel = std::make_unique<Channel>(epoll_loop.get(), std::move(server_socket));
    server_channel->set_read_callback(std::bind(&ServerBase::connection_event_callback, this, std::placeholders::_1));
    server_channel->enable_reading();
}

void ServerBase::connection_event_callback(Channel *channel)
{
    ISocket *server_socket = channel->get_socket();
    int client_socket_fd;
    InetAddress addr;
    logger::logger &log = logger::logger::instance();

    while (true)
    {
        auto [client_socket_fd, addr] = server_socket->accept();

        if (client_socket_fd > 0)
        {
            std::unique_ptr<Channel> new_client_channel = std::make_unique<Channel>(epoll_loop.get(), std::move(std::make_unique<ISocket>(client_socket_fd)));
            new_client_channel->enable_reading();
            new_client_channel->set_read_callback(std::bind(&ServerBase::read_event_callback, this, std::placeholders::_1));
            new_client_channel->set_write_callback(std::bind(&ServerBase::write_event_callback, this, std::placeholders::_1));
            new_client_channel->set_close_callback(std::bind(&ServerBase::close_event_callback, this, std::placeholders::_1));
            epoll_loop->add_channel(std::move(new_client_channel));
            on_connection(epoll_loop->get_channel(client_socket_fd));
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK) // accept over
        {
            break;
        }
        else // error occured
        {
            util::error_assert_with_errno(false, "Accept error : ", false);
            break;
        }
    }
}

void ServerBase::read_event_callback(Channel *channel)
{
    char received_buffer[1024];
    ssize_t received_bytes = -1;

    int socket_fd = channel->get_socket_fd();
    while (true)
    {
        received_bytes = ::recv(socket_fd, received_buffer, sizeof(received_buffer), 0);

        if (received_bytes > 0) // 收到数据，继续读取
        {
            logger::logger::instance().show_info_log("Received data, message: \"" + std::string(received_buffer, received_bytes) + "\" on FD: " + std::to_string(socket_fd));
            channel->append_recv_buffer(std::string(received_buffer, received_bytes));
            read_callback_result result = on_read(channel);
            switch (result)
            {
            case read_callback_result::DO_WRITE:
                write_event_callback(channel);
                break;
            case read_callback_result::DO_CLOSE:
                close_event_callback(channel);
                break;
            case read_callback_result::NONE:
            default:
                break;
            }

            continue;
        }
        else if (received_bytes == 0) // 客户端优雅关闭
        {
            close_event_callback(channel);
            return;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) // 缓存已满
            {
                if (!channel->get_recv_buffer().empty())
                {
                    read_callback_result result = on_read(channel);
                    switch (result)
                    {
                    case read_callback_result::DO_WRITE:
                        write_event_callback(channel);
                        break;
                    case read_callback_result::DO_CLOSE:
                        close_event_callback(channel);
                        break;
                    case read_callback_result::NONE:
                    default:
                        break;
                    }
                }
                return;
            }
            close_event_callback(channel);
            return;
        }
    }
}

void ServerBase::write_event_callback(Channel *channel)
{
    std::string &send_buffer = channel->get_send_buffer();
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
            logger::logger::instance().show_warning_log("Write error on buffered data on FD: " + std::to_string(socket_fd));
            close_event_callback(channel);

            return;
        }
    }

    channel->disable_writing();
}

void ServerBase::close_event_callback(Channel *channel)
{
    logger::logger &log = logger::logger::instance();

    int socket_fd = channel->get_socket_fd();
    on_colse(channel);
    epoll_loop->remove_channel(channel);
}
