#include "CommandHandler.hpp"
#include "ProxyServer.hpp"
#include <iostream>

CommandHandler::CommandHandler(ProxyServer& proxyServer)
    : proxyServer_(proxyServer)
{
    // Register command handlers
    registerCommand("GetUserConnected", std::bind(&CommandHandler::handleGetUserConnected, this, std::placeholders::_1, std::placeholders::_2));
    registerCommand("EchoReply", std::bind(&CommandHandler::handleEchoReply, this, std::placeholders::_1, std::placeholders::_2));
    // Add more command handlers as needed
}

void CommandHandler::registerCommand(const std::string& command, CommandFunction handler)
{
    commandHandlers_[command] = handler;
}

void CommandHandler::handleCommand(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command)
{
    // Extract the command payload
    std::string payload = command.substr(5, command.size() - 6);

    // Find the command handler in the map
    auto it = commandHandlers_.find(payload);
    if (it != commandHandlers_.end())
    {
        // Call the command handler function
        (it->second)(userSocket, command);
    }
    else
    {
        std::cout << "Unknown command: " << payload << std::endl;
    }
}

void CommandHandler::handleGetUserConnected(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command)
{
    // Handle the "GetUserConnected" command
    proxyServer_.notifyUser(userSocket, "[CMD]ConnectedUsers: " + std::to_string(proxyServer_.getUserCount()) + "\n");
}

void CommandHandler::handleEchoReply(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& command)
{
    // Handle the "EchoReply" command
    proxyServer_.notifyUser(userSocket, "[CMD]EchoReply: " + command.substr(11) + "\n");
    // Modify 11 according to the length of the command name
}
