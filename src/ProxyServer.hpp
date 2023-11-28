// ProxyServer.hpp

#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <iostream>
#include <functional>


class ProxyServer
{
public:
    ProxyServer(const std::string &secret);

    void start();

private:
    void startAccept();
    void handleFirstUser();
    void startCommunication();
    void handleCommunication(boost::asio::ip::tcp::socket &sender, boost::asio::ip::tcp::socket &receiver);
    void relayMessage(boost::asio::ip::tcp::socket &receiver, const std::string &message, std::function<void(const boost::system::error_code&)> callback);
    void startTimer();

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket firstUserSocket_;
    boost::asio::ip::tcp::socket secondUserSocket_;
    boost::asio::steady_timer timer_;
    std::string secret_;
    bool hasFirstUserConnected_;
    bool hasSecondUserConnected_;
    boost::asio::streambuf buffer;
};

#endif // PROXY_SERVER_HPP
