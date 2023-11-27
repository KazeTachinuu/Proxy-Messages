// ProxyServer.hpp
#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <string>
#include <vector>
#include <memory>
#include <boost/asio.hpp>

class ProxyServer
{
public:
    explicit ProxyServer(const std::string &secret);
    void start();

private:
    void handleAccept();
    void startReading(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void startWriting(std::shared_ptr<boost::asio::ip::tcp::socket> senderSocket, const std::string &message);

    std::string secret;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    boost::asio::deadline_timer timer;
    boost::asio::streambuf buffer;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> sockets;
};

#endif // PROXY_SERVER_HPP
