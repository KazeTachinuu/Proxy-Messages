// ProxyServer.cpp

#include "ProxyServer.hpp"
#include <iostream>

ProxyServer::ProxyServer()
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 12345))
{
}

void ProxyServer::start()
{
    startAccept();
    io_context_.run();
}

void ProxyServer::startAccept()
{
    acceptor_.async_accept(
        [this](const boost::system::error_code &error, boost::asio::ip::tcp::socket userSocket) {
            if (!error)
            {
                std::cout << "Proxy Server: User connected.\n";

                // Store the connected user socket
                auto newUserSocket = std::make_shared<boost::asio::ip::tcp::socket>(std::move(userSocket));
                userSockets_.push_back(newUserSocket);

                // Notify all users about the new connection (excluding the user itself)
                notifyAllUsers(newUserSocket, "New user connected");

                // Handle the connected user here if needed
            }

            startAccept();
        });
}

void ProxyServer::notifyAllUsers(const std::shared_ptr<boost::asio::ip::tcp::socket>& notifyingUser, const std::string& message)
{
    for (const auto& userSocket : userSockets_)
    {
        // Exclude the notifying user
        if (userSocket != notifyingUser)
        {
            boost::asio::async_write(
                *userSocket,
                boost::asio::buffer(message),
                [](const boost::system::error_code&, std::size_t) {});
        }
    }
}
