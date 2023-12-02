#include "ProxyServer.hpp"

#include <iostream>
#include <string>


ProxyServer::ProxyServer(unsigned short port)
    : acceptor_(
        io_context_,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    port_(port),
    commandHandler_(std::make_unique<CommandHandler>(*this))
{
    // Additional constructor initialization code, if needed
}

void ProxyServer::start()
{
    startAccept();
    std::cout << "Proxy launched on port " << port_ << std::endl;
    // Thread to handle server shutdown
    std::thread shutdownThread([this]() {
        std::string input;
        while (std::getline(std::cin, input))
        {
            // Call the member function instead of the standalone function
            handleInputCommands(input);
        }
    });
    io_context_.run();
}

// Fix the missing return type
void ProxyServer::handleInputCommands(const std::string &input)
{
    if (input == "/help")
    {
        std::cout << "Available commands:\n"
                  << "/exit, /shutdown - stop the server\n"
                  << "/help - print this help message\n"
                  << "/list - list all connected users\n"
                  << "/kill <channel> - disconnect all users from the given channel\n"
                  << "/kick <user> - disconnect the given user\n"
            << std::endl;
    }
    else if (input == "/shutdown" || input == "/exit")
    {
        stopServer();
    }
    else if (input == "/list")
    {//list the channels like so [channel1] User1, User2, User3
        std::cout << "Connected users:\n";
        for (const auto &channel : channels_)
        {
            std::cout << "[" << channel.first << "] ";
            for (const auto &userSocket : channel.second)
            {
                std::cout << connectedUsers_[userSocket] << " " ;
            }
            std::cout << std::endl << std::endl;
        }
    }
    else if (input.find("/kick") == 0)
    {
        //get the username but check if it was provided
        //check if the command have an argument
        std::size_t usernameStart = input.find(' ');
        if (usernameStart == std::string::npos)
        {
            std::cout << "No username provided.\n\n";
            return;
        }
        std::string username = input.substr(usernameStart + 1);


        for (const auto &channel : channels_)
        {
            for (const auto &userSocket : channel.second)
            {
                if (connectedUsers_[userSocket] == username)
                {
                    std::cout << "Kicking " << username << std::endl;
                    notifyUser(userSocket, "[INFO]Kicked by server");
                    notifyUser(userSocket, "[CMD]SHUTDOWN");
                    notifyAllUsers("[INFO]" + username + " kicked by server", userSocket, userChannels_[userSocket]);
                    RemoveUser(userSocket);
                    return;
                }
            }
        }
        std::cout << "User " << username << " not found.\n\n";
    }
    else if (input.find("/kill") == 0)
    {

        std::size_t channelStart = input.find(' ');
        if (channelStart == std::string::npos)
        {
            std::cout << "No channel provided.\n\n";
            return;
        }
        std::string channel = input.substr(channelStart + 1);
        if (channels_.count(channel))
        {
            std::cout << "Killing channel " << channel << std::endl;
            //copy the vector of sockets to a new vector to avoid iterator invalidation
            std::vector<std::shared_ptr<TCP::socket>> sockets = channels_[channel];
            for (const auto &userSocket : sockets)
            {
                notifyUser(userSocket, "[INFO]Killing channel " + channel);
                notifyUser(userSocket, "[CMD]SHUTDOWN");
                RemoveUser(userSocket);
            }

            //remove the channel from the map
            channels_.erase(channel);
        }
        else
        {
            std::cout << "Channel " << channel << " not found.\n\n";
        }
    }
    else
    {
        std::cout << "Unknown command: " << input << std::endl << std::endl;
    }
}


void ProxyServer::notifyUser(
    const std::shared_ptr<TCP::socket> &userSocket,
    const std::string &message)
{
    boost::asio::async_write(
        *userSocket, boost::asio::buffer(message+"\n"),
        [](const boost::system::error_code &, std::size_t) {});
}

void ProxyServer::notifyAllUsers(
    const std::string &message,
    const std::shared_ptr<TCP::socket> &excludedSocket,
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
    const std::shared_ptr<TCP::socket> &newUserSocket,
    const std::string &channel)
{
    std::string username = connectedUsers_[newUserSocket];


    // Store the username and channel in the map
    channels_[channel].push_back(newUserSocket);
    connectedUsers_[newUserSocket] = username;
    userChannels_[newUserSocket] = channel;

    notifyAllUsers("[INFO]New user connected: " + username,
                   newUserSocket, channel);
}

void ProxyServer::startAccept()
{
    acceptor_.async_accept([this](const boost::system::error_code &error,
                                  TCP::socket userSocket) {
        if (!error)
        {
            auto newUserSocket = std::make_shared<TCP::socket>(
                std::move(userSocket));
//            std::cout << "User connected.\n";
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
        notifyUser(userSocket, "[CMD]SHUTDOWN");
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
    const std::shared_ptr<TCP::socket> &newUserSocket)
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
                    notifyUser(newUserSocket, "[INFO]Message received.");
                    break;
                default:
                    std::cerr << "Unknown message type received.\n";
                }
            }
            else
            {
                RemoveUser(newUserSocket);
            }

            handleCommunication(newUserSocket);
        });
}

void ProxyServer::handleMessage(
    const std::shared_ptr<TCP::socket> &userSocket,
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
    const std::shared_ptr<TCP::socket> &userSocket)
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
    const std::shared_ptr<TCP::socket> &userSocket,
    const std::string &message)
{
    //create a username for the user
    //check if the username is already taken

    int id = 1;
    std::string username = "User" + std::to_string(id);
    while (std::find_if(connectedUsers_.begin(), connectedUsers_.end(), [username](const std::pair<std::shared_ptr<TCP::socket>, std::string> &user) { return user.second == username; }) != connectedUsers_.end())
    {
        id++;
        username = "User" + std::to_string(id);
    }
    connectedUsers_[userSocket] = username;

    std::cout << "New user connected: " << username << std::endl;

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
                   "Welcome " + username + " to channel " + channel );

        notifyNewUser(userSocket, channel);
    }
    else
    {
        std::cerr
            << "Invalid initial message format. Missing channel information.\n";
    }
}

void ProxyServer::RemoveUser(const std::shared_ptr<TCP::socket> &userSocket)
{

    std::cout << connectedUsers_[userSocket] << " disconnected.\n";

    notifyAllUsers("[INFO]" + connectedUsers_[userSocket] + " disconnected", userSocket, userChannels_[userSocket]);
    // Remove the user from all the maps
    const std::string channel = userChannels_[userSocket];
    userChannels_.erase(userSocket);
    connectedUsers_.erase(userSocket);
    userSockets_.erase(std::remove(userSockets_.begin(), userSockets_.end(), userSocket), userSockets_.end());
    channels_[channel].erase(std::remove(channels_[channel].begin(), channels_[channel].end(), userSocket), channels_[channel].end());
    if (channels_[channel].empty())
    {
        channels_.erase(channel);
    }


}
