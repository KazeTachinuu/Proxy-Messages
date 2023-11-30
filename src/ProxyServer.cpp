#include "ProxyServer.hpp"
#include <iostream>

ProxyServer::ProxyServer(unsigned short port)
    : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      port_(port), commandHandler_(std::make_unique<CommandHandler>(*this))
{
}

void ProxyServer::start()
{
    startAccept();
    std::cout << "Proxy launched on port " << port_ << std::endl;
    io_context_.run();
}

void ProxyServer::notifyUser(const std::shared_ptr<asio::ip::tcp::socket>& userSocket,
                             const std::string& message)
{
    asio::async_write(
        *userSocket, asio::buffer(message),
        [](const boost::system::error_code&, std::size_t) {});
}

void ProxyServer::notifyAllUsers(const std::string& message,
                                 const std::shared_ptr<asio::ip::tcp::socket>& excludedSocket,
                                 const std::string& channel)
{
    for (const auto& userSocket : channels_[channel])
    {
        if (userSocket != excludedSocket)
        {
            notifyUser(userSocket, message);
        }
    }
}

std::size_t ProxyServer::getUserCount(const std::string& channel)
{
    return channels_.count(channel) ? channels_.at(channel).size() : 0;
}

void ProxyServer::notifyNewUser(const std::shared_ptr<asio::ip::tcp::socket>& newUserSocket)
{
    std::string username = "User" + std::to_string(connectedUsers_.size() + 1);
    connectedUsers_[newUserSocket] = username;

    // Generate a unique username for the new user
    std::cout << "New user connected: " << username << std::endl;

    // Store the username in the map
    channels_["default"].push_back(newUserSocket);
    connectedUsers_[newUserSocket] = username;

    notifyAllUsers("[INFO]New user connected: " + username + "\n", newUserSocket, "default");
}

void ProxyServer::startAccept()
{
    acceptor_.async_accept([this](const boost::system::error_code& error,
                                  asio::ip::tcp::socket userSocket) {
        if (!error)
        {
            auto newUserSocket =
                std::make_shared<asio::ip::tcp::socket>(std::move(userSocket));
            std::cout << "User connected.\n";
            userSockets_.push_back(newUserSocket);
            notifyNewUser(newUserSocket);
            handleCommunication(newUserSocket);
        }

        startAccept();
    });
}

MessageType ProxyServer::getMessageType(const std::string& message)
{
    if (message.size() < 6) // Assuming the minimum length of a message type is
                            // 6 characters, e.g., "[CMD]"
    {
        return MessageType::UNKNOWN;
    }

    auto closingBracketPos = message.find(']');

    if (closingBracketPos != std::string::npos && closingBracketPos < message.size() - 1)
    {
        std::string messageType = message.substr(1, closingBracketPos - 1);

        if (messageType == "CMD")
        {
            return MessageType::CMD;
        }
        else if (messageType == "MSG")
        {
            return MessageType::MSG;
        }
        else if (messageType == "INFO")
        {
            return MessageType::INFO;
        }
    }

    return MessageType::UNKNOWN;
}

void ProxyServer::handleCommunication(const std::shared_ptr<asio::ip::tcp::socket>& newUserSocket)
{
    asio::async_read_until(
        *newUserSocket, receiveBuffer_, '\n',
        [this, newUserSocket](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error)
            {
                std::string message(asio::buffers_begin(receiveBuffer_.data()),
                                    asio::buffers_begin(receiveBuffer_.data()) + bytes_transferred);
                receiveBuffer_.consume(bytes_transferred);

                auto messageType = getMessageType(message);

                switch (messageType)
                {
                case MessageType::CMD:
                    commandHandler_->handleCommand(newUserSocket, message, "default");
                    break;
                case MessageType::MSG:
                    handleMessage(newUserSocket, connectedUsers_[newUserSocket], message, "default");
                    break;
                case MessageType::INFO:
                    notifyUser(newUserSocket, "[INFO]Message received.\n");
                    break;
                default:
                    std::cerr << "Unknown message type received.\n";
                }
            }

            handleCommunication(newUserSocket);
        });
}

void ProxyServer::handleMessage(const std::shared_ptr<asio::ip::tcp::socket>& userSocket,
                                const std::string& username, const std::string& message,
                                const std::string& channel)
{
    std::string formattedMessage = "[" + channel + "]" + username + ": " + message;
    notifyAllUsers(formattedMessage, userSocket, channel);
}
