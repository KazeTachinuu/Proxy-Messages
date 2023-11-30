#pragma once

#include <boost/asio.hpp>
#include <map>
#include <memory>
#include <vector>

#include "CommandHandler.hpp"

enum class MessageType
{
    CMD,
    MSG,
    INFO,
    UNKNOWN

};

namespace asio = boost::asio;

class ProxyServer
{
public:
    ProxyServer(unsigned short port);
    void start();
    void notifyUser(const std::shared_ptr<asio::ip::tcp::socket> &userSocket,
                    const std::string &message);
    void
    notifyAllUsers(const std::string &message,
                   const std::shared_ptr<asio::ip::tcp::socket> &excludedSocket,
                   const std::string &channel);
    std::size_t getUserCount(const std::string &channel);

private:
    void
    notifyNewUser(const std::shared_ptr<asio::ip::tcp::socket> &newUserSocket);
    void startAccept();
    MessageType getMessageType(const std::string &message);
    void handleCommunication(
        const std::shared_ptr<asio::ip::tcp::socket> &newUserSocket);
    void handleMessage(const std::shared_ptr<asio::ip::tcp::socket> &userSocket,
                       const std::string &username, const std::string &message,
                       const std::string &channel);

private:
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    unsigned short port_;
    std::map<std::shared_ptr<asio::ip::tcp::socket>, std::string>
        connectedUsers_;
    std::vector<std::shared_ptr<asio::ip::tcp::socket>> userSockets_;
    asio::streambuf receiveBuffer_;
    std::unique_ptr<CommandHandler> commandHandler_;
    std::map<std::string, std::vector<std::shared_ptr<asio::ip::tcp::socket>>>
        channels_;
};
