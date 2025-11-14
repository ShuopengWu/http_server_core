#include <sys/socket.h>
#include <errno.h>
#include <map>
#include <memory>

#include "src/serverbase.h"

class TCPEchoServer : public ServerBase
{
protected:
    virtual void on_connection(Channel *channel) override;
    virtual read_callback_result on_read(Channel *channel) override;
    virtual void on_write(Channel *channel) override;
    virtual void on_colse(Channel *channel) override;
};

void TCPEchoServer::on_connection(Channel *channel)
{
}

TCPEchoServer::read_callback_result TCPEchoServer::on_read(Channel *channel)
{
    std::string &recv_buffer = channel->get_recv_buffer();
    channel->append_send_buffer(recv_buffer);
    recv_buffer.clear();
    return read_callback_result::DO_WRITE;
}

void TCPEchoServer::on_write(Channel *channel)
{
}

void TCPEchoServer::on_colse(Channel *channel)
{
}
int main()
{
    std::shared_ptr<TCPEchoServer> server = std::make_shared<TCPEchoServer>();
    server->start();
    return EXIT_SUCCESS;
}
