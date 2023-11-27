// ProxyServer.hpp

#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <iostream>

class ProxyServer
{
public:
    ProxyServer(const std::string &secret);

    void start();

private:
    void startAccept();
    void handleFirstUser();
    void handleSecondUser(boost::asio::ip::tcp::socket secondUserSocket);
    void startTimer();
    void handleTimer(const boost::system::error_code &error);
    void relayMessage(boost::asio::ip::tcp::socket &sender,
                      boost::asio::ip::tcp::socket &receiver);

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket firstUserSocket_;  // Store the first user's socket
    boost::asio::ip::tcp::socket secondUserSocket_; // Store the second user's socket
    boost::asio::steady_timer timer_;
    std::string secret_;
    bool hasFirstUserConnected_;
    bool hasSecondUserConnected_;
};

#endif // PROXY_SERVER_HPP

