// ProxyServer.hpp

#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <map>

class ProxyServer
{
public:
    ProxyServer(unsigned short port);

    void start();

private:
    void startAccept();
    void notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket, const std::string &message);
    void handleCommunication(const std::shared_ptr<boost::asio::ip::tcp::socket> &newUserSocket);
    void handleUserMessage(const std::shared_ptr<boost::asio::ip::tcp::socket> &sender,
                           const std::shared_ptr<boost::asio::ip::tcp::socket> &receiver,
                           const std::string &message);
    void handleUserReply(const std::shared_ptr<boost::asio::ip::tcp::socket> &sender,
                         const std::shared_ptr<boost::asio::ip::tcp::socket> &receiver,
                         const std::string &message);

    void notifyAllUsers(const std::shared_ptr<boost::asio::ip::tcp::socket> &excludingUser,
                        const std::string &message);

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> userSockets_;
    std::shared_ptr<boost::asio::ip::tcp::socket> waitingUser_;
    boost::asio::deadline_timer waitingTimer_;
    boost::asio::streambuf receiveBuffer_;
    unsigned short port_;
    std::map<std::shared_ptr<boost::asio::ip::tcp::socket>, std::size_t> userMap_;
};

#endif // PROXY_SERVER_HPP
