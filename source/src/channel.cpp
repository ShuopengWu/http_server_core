#include "channel.h"

#include "iepoll.h"
#include "epollloop.h"

Channel::Channel(EpollLoop *epoll_loop, std::unique_ptr<ISocket> socket) : belong_epoll_loop(epoll_loop),
                                                           socket(std::move(socket)),
                                                           is_in_epoll(false)
{
}

void Channel::enable_reading()
{
    events = EPOLLIN | EPOLLET;
    belong_epoll_loop->update_channel(this);
}

void Channel::enable_writing()
{
    if (!(events & EPOLLOUT))
    {
        events |= EPOLLOUT;
        belong_epoll_loop->update_channel(this);
    }
}

void Channel::disable_writing()
{
    if (events & EPOLLOUT)
    {
        events &= ~EPOLLOUT;
        belong_epoll_loop->update_channel(this);
    }
}

void Channel::set_read_callback(const callback_t &callback)
{
    this->read_callback = callback;
}

void Channel::set_read_callback(callback_t &&callback)
{
    this->read_callback = std::move(callback);
}

void Channel::set_write_callback(const callback_t &callback)
{
    this->write_callback = callback;
}

void Channel::set_write_callback(callback_t &&callback)
{
    this->write_callback = std::move(callback);
}

void Channel::set_close_callback(const callback_t &callback)
{
    this->close_callback = callback;
}

void Channel::set_close_callback(callback_t &&callback)
{
    this->close_callback = std::move(callback);
}

void Channel::handle_event()
{
    if (!is_in_epoll)
	    return;
    logger::logger &log = logger::logger::instance();

    auto do_callback = [this, &log](callback_t &callback)
    {
        if (callback == nullptr)
        {
            log.show_warning_log("No callback function set");
            return;
        }
        callback(this);
    };

    if (!revents)
    {
        log.show_warning_log("Channel is Awakened, but no event occured");
    }

    try
    {
        bool is_event_occured = false;
        if (revents & (EPOLLERR | EPOLLHUP))    // close会销毁channel,优先调用
        {
            is_event_occured = true;
            do_callback(close_callback);
            return;
        }
        if (revents & EPOLLIN)
        {
            is_event_occured = true;
            do_callback(read_callback);
        }
        if (revents & EPOLLOUT)
        {
            is_event_occured = true;
            do_callback(write_callback);
        }
        
        
    }
    catch (const std::exception &e)
    {
        log.show_error_log("Callback find a std exception : " + std::string(e.what()));
    }
    catch (...)
    {
        log.show_error_log("Callback find a unknown exception");
    }
}

ISocket *Channel::get_socket()
{
    return socket.get();
}

int Channel::get_socket_fd() const
{
    return socket->get_socket_fd();
}

std::string &Channel::get_send_buffer()
{
    return send_buffer;
}

void Channel::append_send_buffer(const std::string &data)
{
    send_buffer.append(data);
}

std::string &Channel::get_recv_buffer()
{
    return recv_buffer;
}

void Channel::append_recv_buffer(const std::string &data)
{
    recv_buffer.append(data);
}
