#include "CommandHandler.hpp"

#include <iostream>

#include "ProxyServer.hpp"

CommandHandler::CommandHandler(ProxyServer &proxyServer)
    : proxyServer_(proxyServer)
{
    // Register command handlers
    registerCommand("GETUSERCOUNT",
                    std::bind(&CommandHandler::handleGetUserConnected, this,
                              std::placeholders::_1, std::placeholders::_2,
                              std::placeholders::_3));
    registerCommand("ECHOREPLY",
                    std::bind(&CommandHandler::handleEchoReply, this,
                              std::placeholders::_1, std::placeholders::_2,
                              std::placeholders::_3));
    registerCommand("GETUSERLIST",
                    std::bind(&CommandHandler::handleGetUserList, this,
                              std::placeholders::_1, std::placeholders::_2,
                              std::placeholders::_3));
}

void CommandHandler::registerCommand(const std::string &command,
                                     CommandFunction handler)
{
    commandHandlers_[command] = handler;
}

void CommandHandler::handleCommand(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &command, const std::string &channel)
{
    bool commandHandled = false;

    for (const auto &handler : commandHandlers_)
    {
        const std::string &cmd = handler.first;
        // Check if the command is present in the message
        size_t cmdPos = command.find(cmd);
        if (cmdPos != std::string::npos)
        {
            // Call the corresponding command handler function
            (handler.second)(userSocket, command, channel);
            commandHandled = true;
        }
    }

    if (!commandHandled)
    {
        // If no matching command is found
        std::cout << "Unknown command in message: " << command << std::endl;
    }
}

void CommandHandler::handleGetUserConnected(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &command, const std::string &channel)
{
    // Handle the "GetUserConnected" command
    std::size_t userCount = proxyServer_.getUserCount(channel);
    proxyServer_.notifyUser(userSocket,
                            "[INFO]UserCount: " + std::to_string(userCount)
                                + " in channel " + channel + "\n");
}

void CommandHandler::handleEchoReply(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &command, const std::string &channel)
{
    // Find the position of "ECHOREPLY"
    size_t echoPos = command.find("ECHOREPLY");
    if (echoPos != std::string::npos)
    {
        // Extract the rest of the line after "ECHOREPLY"
        std::string restOfLine = command.substr(
            echoPos
            + 10); // Assuming "ECHOREPLY" is 9 characters long + the space

        // Notify the user with the rest of the lin[e
        proxyServer_.notifyAllUsers("[CMD]ECHOREPLY: " + restOfLine + "\n",
                                    userSocket, channel);
    }
    else
    {
        std::cout << "Command 'ECHOREPLY' not found in the message: " << command
                  << std::endl;
    }
}

void CommandHandler::handleGetUserList(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &command, const std::string &channel)
{
    // Handle the "GetUserList" command
    std::string userList = proxyServer_.getUserList(channel);
    proxyServer_.notifyUser(userSocket, "[INFO]UserList: " + userList + "\n");
}
