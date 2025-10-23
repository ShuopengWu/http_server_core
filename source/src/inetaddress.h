#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <string>
#include <cstring>

#include "logger.h"

namespace
{
static std::string DEFAULT_IP = "";
static constexpr uint16_t DEFAULT_PORT = 10088;
}

class InetAddress
{
public:
    InetAddress(const std::string &ip = DEFAULT_IP, uint16_t port = DEFAULT_PORT);
    sockaddr_in &get_addr();
    const sockaddr_in &get_addr() const;
    std::string to_string() const;
private:
    sockaddr_in addr;
};

#endif
