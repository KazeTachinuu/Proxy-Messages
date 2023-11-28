// ProxyServer.cpp

#include "ProxyServer.hpp"
#include <iostream>

ProxyServer::ProxyServer()
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 12345)),
    waitingTimer_(io_context_)
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

                // If there's a waiting user, notify the waiting user about the new connection
                if (waitingUser_)
                {
                    notifyUser(waitingUser_, "\nAnother user has connected.");
                    waitingTimer_.cancel();
                }

                // Store the connected user socket
                auto newUserSocket = std::make_shared<boost::asio::ip::tcp::socket>(std::move(userSocket));
                userSockets_.push_back(newUserSocket);

                // Notify the connected user about the existing users
                if (userSockets_.size() > 1)
                {
                    notifyUser(newUserSocket, "\nOther users are already connected.");
                }
                else
                {
                    notifyUser(newUserSocket, "\nNo other users are currently connected. Waiting 30 seconds for connections.");
                    // Start waiting timer for the first user
                    waitingUser_ = newUserSocket;
                    waitingTimer_.expires_from_now(boost::posix_time::seconds(30));
                    waitingTimer_.async_wait([this](const boost::system::error_code &error) {
                        if (!error)
                        {
                            std::cout << "No other user connected within 30 seconds. Closing waiting user connection.\n";
                            // Notify the waiting user about the timeout
                            notifyUser(waitingUser_, "\nNo other user connected within 30 seconds. Closing connection.");
                            // Close the socket
                            waitingUser_->close();
                            waitingUser_ = nullptr;
                        }
                    });
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
