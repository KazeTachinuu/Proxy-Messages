#include "ProxyServer.hpp"
#include "CommandHandler.hpp"


ProxyServer::ProxyServer(unsigned short port)
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    port_(port),
    commandHandler_(std::make_unique<CommandHandler>(*this)) // Initialize the CommandHandler with a reference to this ProxyServer
{
}

void ProxyServer::start()
{
    startAccept();
    std::cout << "Proxy launched on port " << port_ << std::endl;
    io_context_.run();
}

void ProxyServer::notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message)
{
    boost::asio::async_write(
        *userSocket, boost::asio::buffer(message),
        [](const boost::system::error_code&, std::size_t) {});
}

void ProxyServer::notifyAllUsers(const std::string& message, const std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>>& excludedSockets)
{
    for (const auto& userSocket : userSockets_)
    {
        // Check if the current userSocket is not in the excludedSockets list
        if (std::find(excludedSockets.begin(), excludedSockets.end(), userSocket) == excludedSockets.end())
        {
            notifyUser(userSocket, message);
        }
    }
}

std::size_t ProxyServer::getUserCount() const
{
    return userSockets_.size();
}

void ProxyServer::startAccept()
{
    acceptor_.async_accept(
        [this](const boost::system::error_code& error, boost::asio::ip::tcp::socket userSocket) {
            if (!error)
            {
                auto newUserSocket = std::make_shared<boost::asio::ip::tcp::socket>(std::move(userSocket));
                std::cout << "User connected.\n";

                // Store the connected user socket
                userSockets_.push_back(newUserSocket);

                // Notify all users about the new connection
                notifyAllUsers("User connected.\n", {newUserSocket});

                // Handle the communication for the connected user
                handleCommunication(newUserSocket);
            }

            startAccept();
        });
}

MessageType getMessageType(const std::string& message)
{
    if (message.size() < 5)
    {
        return MessageType::UNKNOWN;
    }
    std::string sub = message.substr(0, 5);
    if (sub == "[CMD]")
    {
        return MessageType::CMD;
    }
    else if (sub == "[MSG]")
    {
        return MessageType::MSG;
    }
    else
    {
        return MessageType::UNKNOWN;
    }
}

void ProxyServer::handleCommunication(const std::shared_ptr<boost::asio::ip::tcp::socket>& newUserSocket)
{
    boost::asio::async_read_until(
        *newUserSocket, receiveBuffer_, '\n',
        [this, newUserSocket](const boost::system::error_code& error, std::size_t bytes_received) {
            if (!error && bytes_received > 0)
            {
                std::istream is(&receiveBuffer_);
                std::string message;
                std::getline(is, message);

                std::cout << "Received from User:\n" << message << std::endl;


                MessageType messageType = getMessageType(message);
                switch (messageType)
                {
                case MessageType::CMD:
                    commandHandler_->handleCommand(newUserSocket, message); // Use -> instead of .
                    break;
                case MessageType::MSG:
                    handleMessage(newUserSocket, message);
                    break;
                case MessageType::UNKNOWN:
                    std::cout << "Invalid message format: " << message << std::endl;
                    break;
                }
                // Continue reading messages
                handleCommunication(newUserSocket);
            }
            else
            {
                std::cout << "Error or no bytes received in handleCommunication.\n";

                // Handle disconnection or error
                userSockets_.erase(
                    std::remove(userSockets_.begin(), userSockets_.end(), newUserSocket),
                    userSockets_.end());

                // Notify all users about the disconnection
                notifyAllUsers("User disconnected.\n", {newUserSocket});
            }
        });
}

void ProxyServer::handleMessage(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message)
{
    std::string mes = message.substr(5, message.size() - 6);
    notifyAllUsers(mes + "\n", {userSocket});
}