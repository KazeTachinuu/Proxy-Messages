// ProxyServer.hpp

#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <vector>

class ProxyServer
{
public:
    ProxyServer();

    void start();

private:
    void startAccept();
    void notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message);
    void handleWaitingUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& waitingUser);

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;

    // Store connected user sockets
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> userSockets_;

    // Timer for waiting user
    boost::asio::deadline_timer waitingTimer_;
    std::shared_ptr<boost::asio::ip::tcp::socket> waitingUser_;
};

#endif // PROXY_SERVER_HPP
