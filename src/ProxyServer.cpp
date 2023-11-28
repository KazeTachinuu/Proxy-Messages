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

                // Notify only the connected user about the existing users
                if (userSockets_.size() > 1)
                {
                    notifyUser(newUserSocket, "Other users are already connected.");
                }
                else
                {
                    notifyUser(newUserSocket, "No other users are currently connected. Waiting for connections.");
                }

                // Handle the connected user here if needed
            }

            startAccept();
        });
}

void ProxyServer::notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message)
{
    boost::asio::async_write(
        *userSocket,
        boost::asio::buffer(message),
        [](const boost::system::error_code&, std::size_t) {});
}
