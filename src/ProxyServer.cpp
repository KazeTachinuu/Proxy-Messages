// ProxyServer.cpp

#include "ProxyServer.hpp"
#include <iostream>
#include <map>

ProxyServer::ProxyServer(unsigned short port)
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    waitingTimer_(io_context_),
    port_(port)  // Initialize the port member
{
}

void ProxyServer::start()
{
    startAccept();
    std::cout << "Proxy launched on port " << port_ << std::endl;
    io_context_.run();
}

void ProxyServer::startAccept()
{
    acceptor_.async_accept(
        [this](const boost::system::error_code &error, boost::asio::ip::tcp::socket userSocket) {
            if (!error)
            {
                auto newUserSocket = std::make_shared<boost::asio::ip::tcp::socket>(std::move(userSocket));
                std::cout << "Proxy Server: User " + std::to_string(userSockets_.size()+1) + " connected.\n";
                notifyUser(newUserSocket,"User " + std::to_string(userSockets_.size()+1) + " connected.\n");

                // Store the connected user socket
                userSockets_.push_back(newUserSocket);

                // Notify the connected user about the existing users
                if (userSockets_.size() > 1)
                {
                    // Notify the new user about the existing users
                    notifyUser(newUserSocket, std::to_string(userSockets_.size()-1)+" users are already connected.\n");

                    // Add the new user to the user map
                    userMap_[newUserSocket] = userSockets_.size();

                    notifyAllUsers(newUserSocket,"User " + std::to_string(userMap_[newUserSocket]) + " has connected.\n");
                }
                else
                {
                    notifyUser(newUserSocket,
                               "No other users are currently connected. Waiting 30 seconds "
                               "for connections.\n");

                    // Start waiting timer for the first user
                    waitingUser_ = newUserSocket;
                    waitingTimer_.expires_from_now(boost::posix_time::seconds(30));
                    waitingTimer_.async_wait(
                        [this](const boost::system::error_code &error) {
                            if (!error)
                            {
                                std::cout
                                    << "No other user connected within 30 seconds. "
                                       "Closing waiting user connection.\n";
                                // Notify the waiting user about the timeout
                                notifyUser(waitingUser_,
                                           "No other user connected within 30 "
                                           "seconds. Closing connection.\n");
                                // Close the socket
                                waitingUser_->close();
                                waitingUser_ = nullptr;
                                userSockets_.clear();
                            }
                        });
                }

                // Handle the connected user here if needed
            }

            startAccept();
        });
}

void ProxyServer::notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket, const std::string &message)
{
    boost::asio::async_write(
        *userSocket, boost::asio::buffer(message),
        [](const boost::system::error_code &, std::size_t) {});
}


void ProxyServer::notifyAllUsers(const std::shared_ptr<boost::asio::ip::tcp::socket> &excludingUser, const std::string &message)
{
    for (const auto &userSocket : userSockets_)
    {
        if (userSocket != excludingUser)
        {
            boost::asio::async_write(
                *userSocket,
                boost::asio::buffer(message),
                [](const boost::system::error_code &, std::size_t) {});
        }
    }
}

