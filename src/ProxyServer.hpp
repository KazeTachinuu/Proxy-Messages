// ProxyServer.hpp

#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <boost/asio.hpp>
#include <memory>
#include <vector>

class ProxyServer
{
public:
    ProxyServer(unsigned short port);

    void start();

private:
    void startAccept();
    void notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message);
    void handleWaitingUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& waitingUser);
    void handleConnectedUsers(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocketA,
                              const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocketB);
    void handleUserMessage(const std::string& sender_name,
                           const std::shared_ptr<boost::asio::ip::tcp::socket>& receiver,
                           const std::string& message);

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;

    // Store connected user sockets
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> userSockets_;

    // Timer for waiting user
    boost::asio::deadline_timer waitingTimer_;
    std::shared_ptr<boost::asio::ip::tcp::socket> waitingUser_;
    unsigned short port_;
};

#endif // PROXY_SERVER_HPP
