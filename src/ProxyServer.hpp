// ProxyServer.hpp

#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <boost/asio.hpp>

class ProxyServer
{
public:
    ProxyServer(unsigned short port);

    void start();

private:
    void startAccept();
    void notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message);
    void handleConnectedUsers(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocketA,
                              const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocketB);
    void handleUserMessage(const std::string& sender_name,
                           const std::shared_ptr<boost::asio::ip::tcp::socket>& sender,
                           const std::string& receiver_name,
                           const std::shared_ptr<boost::asio::ip::tcp::socket>& receiver,
                           const std::string& message);
    void handleUserReply(const std::string& sender_name,
                         const std::shared_ptr<boost::asio::ip::tcp::socket>& sender,
                         const std::string& receiver_name,
                         const std::shared_ptr<boost::asio::ip::tcp::socket>& receiver);

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> userSockets_;
    std::shared_ptr<boost::asio::ip::tcp::socket> waitingUser_;
    boost::asio::deadline_timer waitingTimer_;
    boost::asio::streambuf receiveBuffer_;
    unsigned short port_;
};

#endif // PROXY_SERVER_HPP
