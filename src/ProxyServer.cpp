// ProxyServer.cpp

#include "ProxyServer.hpp"

ProxyServer::ProxyServer(const std::string &secret)
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 12345)),
      firstUserSocket_(io_context_),
      secondUserSocket_(io_context_),
      timer_(io_context_),
      secret_(secret),
      hasFirstUserConnected_(false),
      hasSecondUserConnected_(false)
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
        firstUserSocket_,
        [this](const boost::system::error_code &error) {
            if (!error)
            {
                std::cout << "Proxy Server accepted connection with secret: " << secret_ << std::endl;
                handleFirstUser();
            }

            startAccept();
        });
}

void ProxyServer::handleFirstUser()
{
    if (!hasFirstUserConnected_)
    {
        hasFirstUserConnected_ = true;

        // Start the timer for 30 seconds
        startTimer();

        acceptor_.async_accept(
            secondUserSocket_,
            [this](const boost::system::error_code &error) {
                if (!error)
                {
                    // Cancel the timer since the second user has connected
                    timer_.cancel();

                    std::cout << "Second user connected within 30 seconds. Informing users and handling communication.\n";

                    // Inform both users about the connection
                    relayMessage(firstUserSocket_, secondUserSocket_);
                    relayMessage(secondUserSocket_, firstUserSocket_);

                    // Handle communication between connected users
                    handleSecondUser(std::move(secondUserSocket_));
                }
            });
    }
    else
    {
        // Logic for handling multiple first user connections, if needed
    }
}

void ProxyServer::handleSecondUser(boost::asio::ip::tcp::socket secondUserSocket)
{
    // Implement logic for handling communication between connected users
    // You have access to both firstUserSocket_ and secondUserSocket here

    // For simplicity, let's implement a basic message exchange
    relayMessage(firstUserSocket_, secondUserSocket);
    relayMessage(secondUserSocket, firstUserSocket_);
}

void ProxyServer::relayMessage(boost::asio::ip::tcp::socket &sender,
                               boost::asio::ip::tcp::socket &receiver)
{
    // Read message from the sender
    boost::asio::streambuf buffer;
    boost::asio::read_until(sender, buffer, '\n');
    std::istream is(&buffer);
    std::string message;
    std::getline(is, message);

    // Print the message in the proxy's terminal
    std::cout << "Proxy Server received message: " << message << std::endl;

    // Relay the message to the receiver
    boost::asio::write(receiver, boost::asio::buffer(message + "\n"));
}

void ProxyServer::startTimer()
{
    timer_.expires_after(std::chrono::seconds(30));
    timer_.async_wait([this](const boost::system::error_code &error) {
        if (!error)
        {
            // Timer expired, no second user connected
            std::cout << "No second user connected within 30 seconds. Closing the connection.\n";
            hasFirstUserConnected_ = false;
            // Close the first user's connection
            boost::system::error_code ec;
            firstUserSocket_.close(ec);
        }
    });
}

