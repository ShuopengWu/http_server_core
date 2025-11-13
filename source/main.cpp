#include <sys/socket.h>
#include <errno.h>
#include <map>
#include <memory>

#include "src/serverbase.h"

class TCPEchoServer : public ServerBase
{
private:
    enum class echo_status
    {
        ALL_ECHO,
        PART_ECHO,
        ERROR_ECHO
    };

protected:
    virtual void on_connection(Channel *channel) override;
    virtual void on_read(Channel *channel) override;
    virtual void on_write(Channel *channel) override;
    virtual void on_colse(Channel *channel) override;
};

void TCPEchoServer::on_connection(Channel *channel)
{
}

void TCPEchoServer::on_read(Channel *channel)
{
    std::string &recv_buffer = channel->get_recv_buffer();
    channel->append_send_buffer(recv_buffer);
    recv_buffer.clear();
    channel->enable_writing();
}

void TCPEchoServer::on_write(Channel *channel)
{
}

void TCPEchoServer::on_colse(Channel *channel)
{
}
int main()
{
    std::unique_ptr<TCPEchoServer> server = std::make_unique<TCPEchoServer>();
    server->start();
    return EXIT_SUCCESS;
}
