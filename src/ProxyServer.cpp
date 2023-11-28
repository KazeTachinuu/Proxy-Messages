// ProxyServer.cpp

#include "ProxyServer.hpp"
#include <iostream>

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
                std::cout << "Proxy Server: User connected.\n";

                // If there's a waiting user, notify the waiting user about the new connection
                if (waitingUser_)
                {
                    notifyUser(waitingUser_, "Another user has connected.\n");
                    waitingTimer_.cancel();
                }

                // Store the connected user socket
                auto newUserSocket = std::make_shared<boost::asio::ip::tcp::socket>(std::move(userSocket));
                userSockets_.push_back(newUserSocket);

                // Notify the connected user about the existing users
                if (userSockets_.size() > 1)
                {
                    notifyUser(newUserSocket, "Other users are already connected.\n");

                    // If there are two users connected, handle communication between them
                    if (userSockets_.size() == 2)
                    {
                        handleConnectedUsers(userSockets_[0], userSockets_[1]);
                    }
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

void ProxyServer::notifyUser(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocket, const std::string& message)
{
    boost::asio::async_write(
        *userSocket, boost::asio::buffer(message),
        [](const boost::system::error_code &, std::size_t) {});
}

void ProxyServer::handleConnectedUsers(const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocketA,
                                       const std::shared_ptr<boost::asio::ip::tcp::socket>& userSocketB)
{
    // Inform user A that it has been connected to user B
    notifyUser(userSocketA, "You are now connected to User B.\n");

    // Inform user B that it has been connected to user A
    notifyUser(userSocketB, "You are now connected to User A.\n");

    // Notify user B with "hello" from user A
    handleUserMessage("User A",userSocketB,"hello\n");

}

void ProxyServer::handleUserMessage(const std::string& sender_name,
                                    const std::shared_ptr<boost::asio::ip::tcp::socket>& receiver,
                                    const std::string& message)
{
    // Construct the complete message including sender's name
    std::string completeMessage = sender_name + ": " + message;

    std::cout << completeMessage << std::endl;
    // Notify the receiver with the complete message
    notifyUser(receiver, completeMessage);
}
