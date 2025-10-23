#include "inetaddress.h"

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (ip.empty() || ip == "0.0.0.0")
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        if (!inet_pton(AF_INET, ip.c_str(), &addr.sin_addr))
        {
            logger::logger::instance().show_waring_log("Invalid IP address provided: " + ip + ". Defaulting to INADDR_ANY.");
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
        }
    }
}

sockaddr_in &InetAddress::get_addr()
{
    return addr;
}

const sockaddr_in &InetAddress::get_addr() const
{
    return addr;
}

std::string InetAddress::to_string() const
{
    char ip[INET_ADDRSTRLEN];
    const char *result = inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    if (result == nullptr)
        return "Unknown ip address";
    
    uint16_t port = ntohs(addr.sin_port);
    return std::string(ip) + ":" + std::to_string(port);
}
