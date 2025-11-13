#ifndef CHANNEL_H
#define CHANNEL_H

#include <cstdint>
#include <cstdlib>
#include <functional>

#include "util.h"
#include "isocket.h"

class EpollLoop;
class Channel;

using callback_t = std::function<void(Channel *channel)>;

class Channel
{
DECLARE_PROPERTY(std::uint32_t, events)
DECLARE_PROPERTY(std::uint32_t, revents)
DECLARE_PROPERTY(bool, is_in_epoll)
public:
    Channel(EpollLoop *epoll_loop, std::unique_ptr<ISocket> socket);
    ISocket *get_socket();
    int get_socket_fd() const;
    std::string &get_send_buffer();
    void append_send_buffer(const std::string &data);
    std::string &get_recv_buffer();
    void append_recv_buffer(const std::string &data);

    void enable_reading();
    void enable_writing();
    void disable_writing();

    void set_read_callback(const callback_t &callback);
    void set_read_callback(callback_t &&callback);
    void set_write_callback(const callback_t &callback);
    void set_write_callback(callback_t &&callback);
    void set_close_callback(const callback_t &callback);
    void set_close_callback(callback_t &&callback);

    void handle_event();
private:
    EpollLoop *belong_epoll_loop;
    std::unique_ptr<ISocket> socket;
    std::string recv_buffer;
    std::string send_buffer;
    callback_t read_callback;
    callback_t write_callback;
    callback_t close_callback;
};

#endif