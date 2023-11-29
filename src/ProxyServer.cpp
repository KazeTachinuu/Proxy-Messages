#include "ProxyServer.hpp"
#include "CommandHandler.hpp"

namespace asio = boost::asio;

ProxyServer::ProxyServer(unsigned short port)
    : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
    port_(port),
    commandHandler_(std::make_unique<CommandHandler>(*this))
{
}

void ProxyServer::start()
{
    startAccept();
    std::cout << "Proxy launched on port " << port_ << std::endl;
    io_context_.run();
}

void ProxyServer::notifyUser(const std::shared_ptr<asio::ip::tcp::socket>& userSocket, const std::string& message)
{
    asio::async_write(
        *userSocket, asio::buffer(message),
        [](const boost::system::error_code&, std::size_t) {});
}

void ProxyServer::notifyAllUsers(const std::string& message, const std::vector<std::shared_ptr<asio::ip::tcp::socket>>& excludedSockets)
{
    for (const auto& userSocket : userSockets_)
    {
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

void ProxyServer::notifyNewUser(const std::shared_ptr<asio::ip::tcp::socket>& newUserSocket)
{
    notifyUser(newUserSocket, "[INFO]NewUserConnected\n");
    notifyAllUsers("New user connected.\n", {newUserSocket});
}

void ProxyServer::startAccept()
{
    acceptor_.async_accept(
        [this](const boost::system::error_code& error, asio::ip::tcp::socket userSocket) {
            if (!error)
            {
                auto newUserSocket = std::make_shared<asio::ip::tcp::socket>(std::move(userSocket));
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
    if (message.size() < 6) // Assuming the minimum length of a message type is 6 characters, e.g., "[CMD]"
    {
        return MessageType::UNKNOWN;
    }

    // Find the position of the first closing bracket (]) in the response
    auto closingBracketPos = message.find(']');

    if (closingBracketPos != std::string::npos && closingBracketPos < message.size() - 1)
    {
        // Extract the message type
        std::string messageType = message.substr(1, closingBracketPos - 1);

        // Map the message type to the corresponding enum value
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
                    commandHandler_->handleCommand(newUserSocket, message);
                    break;
                case MessageType::MSG:
                    handleMessage(newUserSocket, message);
                    break;
                case MessageType::UNKNOWN:
                    std::cout << "Invalid message format: " << message << std::endl;
                    break;
                }

                handleCommunication(newUserSocket);
            }
            else
            {
                std::cout << "User disconnected.\n";
                userSockets_.erase(
                    std::remove(userSockets_.begin(), userSockets_.end(), newUserSocket),
                    userSockets_.end());
                notifyAllUsers("User disconnected.\n", {newUserSocket});
            }
        });
}

void ProxyServer::handleMessage(const std::shared_ptr<asio::ip::tcp::socket>& userSocket, const std::string& message)
{
    std::string mes = message.substr(5, message.size() - 6);
    notifyAllUsers(mes + "\n", {userSocket});
}
