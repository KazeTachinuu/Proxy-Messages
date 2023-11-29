#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <memory>

class CommandHandler; // Forward declaration

enum class MessageType
{
    CMD,
    MSG,
    UNKNOWN
};

class ProxyServer
{
public:
    ProxyServer(unsigned short port);

    void start();

    void notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message);
    void notifyAllUsers(const std::string& message, const std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>>& excludedSockets = {});
    std::size_t getUserCount() const;

private:
    void startAccept();
    void handleCommunication(const std::shared_ptr<boost::asio::ip::tcp::socket>& newUserSocket);
    void handleMessage(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message);

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    unsigned short port_;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> userSockets_;
    boost::asio::streambuf receiveBuffer_;
    std::unique_ptr<CommandHandler> commandHandler_; // Use a pointer
};

#endif // PROXY_SERVER_HPP
