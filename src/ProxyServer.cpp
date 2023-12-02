#include "ProxyServer.hpp"

#include <iostream>
#include <string>

ProxyServer::ProxyServer(unsigned short port)
    : acceptor_(
        io_context_,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , port_(port)
    , commandHandler_(std::make_unique<CommandHandler>(*this))
{}

void ProxyServer::start()
{
    startAccept();
    std::cout << "Proxy launched on port " << port_ << std::endl;
    // Thread to handle server shutdown
    std::thread shutdownThread([this]() {
        std::string input;
        while (std::getline(std::cin, input))
        {
            if (input == "exit")
            {
                stopServer();
                break;
            }
        }
    });
    io_context_.run();
}

void ProxyServer::notifyUser(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &message)
{
    boost::asio::async_write(
        *userSocket, boost::asio::buffer(message),
        [](const boost::system::error_code &, std::size_t) {});
}

void ProxyServer::notifyAllUsers(
    const std::string &message,
    const std::shared_ptr<boost::asio::ip::tcp::socket> &excludedSocket,
    const std::string &channel)
{
    for (const auto &userSocket : channels_[channel])
    {
        if (userSocket != excludedSocket)
        {
            notifyUser(userSocket, message);
        }
    }
}

std::size_t ProxyServer::getUserCount(const std::string &channel)
{
    return channels_.count(channel) ? channels_.at(channel).size() : 0;
}

void ProxyServer::notifyNewUser(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &newUserSocket,
    const std::string &channel)
{
    std::string username = connectedUsers_[newUserSocket];

    // Generate a unique username for the new user
    std::cout << "New user connected: " << username << std::endl;

    // Store the username and channel in the map
    channels_[channel].push_back(newUserSocket);
    connectedUsers_[newUserSocket] = username;
    userChannels_[newUserSocket] = channel;

    notifyAllUsers("[INFO]New user connected: " + username + "\n",
                   newUserSocket, channel);
}

void ProxyServer::startAccept()
{
    acceptor_.async_accept([this](const boost::system::error_code &error,
                                  boost::asio::ip::tcp::socket userSocket) {
        if (!error)
        {
            auto newUserSocket = std::make_shared<boost::asio::ip::tcp::socket>(
                std::move(userSocket));
            std::cout << "User connected.\n";
            userSockets_.push_back(newUserSocket);
            handleNewUser(newUserSocket);
        }

        startAccept();
    });
}

void ProxyServer::stopServer()
{
    std::cout << "Stopping server...\n";
    // Close all the user sockets
    for (const auto &userSocket : userSockets_)
    {
        notifyUser(userSocket, "[CMD]SHUTDOWN\n");
    }
    exit(0);
}

MessageType ProxyServer::getMessageType(const std::string &message)
{
    if (message.size() < 6) // Assuming the minimum length of a message type is
                            // 6 characters, e.g., "[CMD]"
    {
        return MessageType::UNKNOWN;
    }

    auto closingBracketPos = message.find(']');

    if (closingBracketPos != std::string::npos
        && closingBracketPos < message.size() - 1)
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

void ProxyServer::handleCommunication(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &newUserSocket)
{
    const std::string channel = userChannels_[newUserSocket];
    boost::asio::async_read_until(
        *newUserSocket, receiveBuffer_, '\n',
        [this, newUserSocket, channel](const boost::system::error_code &error,
                                       std::size_t bytes_transferred) {
            if (!error)
            {
                std::string message(
                    boost::asio::buffers_begin(receiveBuffer_.data()),
                    boost::asio::buffers_begin(receiveBuffer_.data())
                        + bytes_transferred);
                receiveBuffer_.consume(bytes_transferred);

                std::cout << "Received from [" << connectedUsers_[newUserSocket]
                          << "]:\n"
                          << message << std::endl;
                auto messageType = getMessageType(message);

                switch (messageType)
                {
                case MessageType::CMD:
                    commandHandler_->handleCommand(newUserSocket, message,
                                                   channel);
                    break;
                case MessageType::MSG:
                    handleMessage(newUserSocket, message);
                    break;
                case MessageType::INFO:
                    notifyUser(newUserSocket, "[INFO]Message received.\n");
                    break;
                default:
                    std::cerr << "Unknown message type received.\n";
                }
            }
            else
            {
                std::cerr << connectedUsers_[newUserSocket]
                          << " disconnected.\n";

                notifyAllUsers("[INFO]" + connectedUsers_[newUserSocket]
                                   + " disconnected.\n",
                               newUserSocket, channel);
                // Remove the user from all the maps
                channels_[channel].erase(std::remove(channels_[channel].begin(),
                                                     channels_[channel].end(),
                                                     newUserSocket),
                                         channels_[channel].end());
                connectedUsers_.erase(newUserSocket);
                userChannels_.erase(newUserSocket);
                userSockets_.erase(std::remove(userSockets_.begin(),
                                               userSockets_.end(),
                                               newUserSocket),
                                   userSockets_.end());
            }

            handleCommunication(newUserSocket);
        });
}

void ProxyServer::handleMessage(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &message)
{
    // Use the user's channel information stored during the initial connection
    const std::string channel = userChannels_[userSocket];
    const std::string senderUsername = connectedUsers_[userSocket];

    // Modify the message format to include the sender's username
    std::string formattedMessage = "[" + senderUsername + "]" + message;

    notifyAllUsers(formattedMessage, userSocket, channel);
}

void ProxyServer::handleNewUser(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket)
{
    // Read the initial message to determine the user's channel
    boost::asio::async_read_until(
        *userSocket, receiveBuffer_, '\n',
        [this, userSocket](const boost::system::error_code &error,
                           std::size_t bytes_transferred) {
            if (!error)
            {
                std::string message(
                    boost::asio::buffers_begin(receiveBuffer_.data()),
                    boost::asio::buffers_begin(receiveBuffer_.data())
                        + bytes_transferred);
                receiveBuffer_.consume(bytes_transferred);

                // Assuming the initial message contains the channel information
                handleInitialUserMessage(userSocket, message);
            }

            // Continue listening for messages from the user
            handleCommunication(userSocket);
        });
}

void ProxyServer::handleInitialUserMessage(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &message)
{
    std::string username = "User" + std::to_string(connectedUsers_.size() + 1);
    connectedUsers_[userSocket] = username;

    // Extract the channel information from the initial message
    std::cout << "Received channel assignement from [" << username << "]:\n"
              << message << std::endl;
    std::size_t channelStart = message.find('[');
    std::size_t channelEnd = message.find(']');

    if (channelStart != std::string::npos && channelEnd != std::string::npos
        && channelEnd > channelStart)
    {
        std::string channel =
            message.substr(channelStart + 1, channelEnd - channelStart - 1);

        // Associate the user's socket with the chosen channel
        userChannels_[userSocket] = channel;
        std::cout << username << " assigned to channel " << channel
                  << std::endl;

        notifyUser(userSocket,
                   "Welcome " + username + " to channel " + channel + "\n");

        notifyNewUser(userSocket, channel);
    }
    else
    {
        std::cerr
            << "Invalid initial message format. Missing channel information.\n";
    }
}
