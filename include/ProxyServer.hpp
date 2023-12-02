#ifndef PROXYSERVER_HPP
#define PROXYSERVER_HPP

#include <boost/asio.hpp>
#include <iostream>
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

class ProxyServer
{
public:
    ProxyServer(unsigned short port);
    void start();

    // Function to notify a specific user
    void
    notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
               const std::string &message);

    // Function to notify all users except the excluded one
    void notifyAllUsers(
        const std::string &message,
        const std::shared_ptr<boost::asio::ip::tcp::socket> &excludedSocket,
        const std::string &channel);

    // Function to get the user count in a specific channel
    std::size_t getUserCount(const std::string &channel);

    // Function to notify about a new user and assign a channel
    void notifyNewUser(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &newUserSocket,
        const std::string &channel);

private:
    void startAccept();

    // Function to handle communication with a user
    void handleCommunication(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &newUserSocket);

    // Function to handle messages based on type
    void handleMessage(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
        const std::string &message);

    // Function to handle a new user connection
    void handleNewUser(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket);

    // Function to get the type of message (CMD, MSG, INFO, UNKNOWN)
    MessageType getMessageType(const std::string &message);

    // Function to handle the initial message from a user and assign a channel
    void handleInitialUserMessage(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
        const std::string &message);

    // Function to handle continuous user messages
    void handleUserMessages(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket);

    void stopServer();

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::streambuf receiveBuffer_;

    unsigned short port_;
    std::unique_ptr<CommandHandler> commandHandler_;

    // Map to store user sockets associated with their usernames
    std::map<std::shared_ptr<boost::asio::ip::tcp::socket>, std::string>
        connectedUsers_;

    // Map to store user sockets associated with their channels
    std::map<std::shared_ptr<boost::asio::ip::tcp::socket>, std::string>
        userChannels_;

    // Map to store channels and associated user sockets
    std::map<std::string,
             std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>>>
        channels_;

    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> userSockets_;
};

#endif // PROXYSERVER_HPP
