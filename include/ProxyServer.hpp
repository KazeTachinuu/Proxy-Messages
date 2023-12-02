#ifndef PROXYSERVER_HPP
#define PROXYSERVER_HPP

#include <boost/asio.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

using TCP = boost::asio::ip::tcp;

#include "CommandHandler.hpp"

enum class MessageType
{
    CMD,
    MSG,
    INFO,
    UNKNOWN
};

class ProxyServer
{
public:
    ProxyServer(unsigned short port);
    void start();
    void notifyUser(const std::shared_ptr<TCP::socket> &userSocket, const std::string &message);
    void notifyAllUsers(const std::string &message, const std::shared_ptr<TCP::socket> &excludedSocket, const std::string &channel);
    std::size_t getUserCount(const std::string &channel);
    void notifyNewUser(const std::shared_ptr<TCP::socket> &newUserSocket, const std::string &channel);

private:
    void startAccept();
    void handleCommunication(const std::shared_ptr<TCP::socket> &newUserSocket);
    void handleMessage(const std::shared_ptr<TCP::socket> &userSocket, const std::string &message);
    void handleNewUser(const std::shared_ptr<TCP::socket> &userSocket);
    MessageType getMessageType(const std::string &message);
    void handleInitialUserMessage(const std::shared_ptr<TCP::socket> &userSocket, const std::string &message);
    void handleUserMessages(const std::shared_ptr<TCP::socket> &userSocket);
    void stopServer();
    void RemoveUser(const std::shared_ptr<TCP::socket> &userSocket);
    void handleInputCommands(const std::string &input);

private:
    boost::asio::io_context io_context_;
    TCP::acceptor acceptor_;
    boost::asio::streambuf receiveBuffer_;

    unsigned short port_;
    std::unique_ptr<CommandHandler> commandHandler_;
    std::map<std::shared_ptr<TCP::socket>, std::string> connectedUsers_;
    std::map<std::shared_ptr<TCP::socket>, std::string> userChannels_;
    std::map<std::string, std::vector<std::shared_ptr<TCP::socket>>> channels_;
    std::vector<std::shared_ptr<TCP::socket>> userSockets_;
};

#endif // PROXYSERVER_HPP
