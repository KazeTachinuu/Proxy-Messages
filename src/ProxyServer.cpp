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
                    relayMessage(firstUserSocket_, "Second user connected. You are now connected to each other.\n", [](const boost::system::error_code&) {});
                    relayMessage(secondUserSocket_, "You are now connected to the first user.\n", [](const boost::system::error_code&) {});

                    // Start communication between connected users
                    startCommunication();
                }
            });
    }
    else
    {
        // Logic for handling multiple first user connections, if needed
    }
}

void ProxyServer::startCommunication()
{
    // Start asynchronous operations to continuously read and write messages
    handleCommunication(firstUserSocket_, secondUserSocket_);
    handleCommunication(secondUserSocket_, firstUserSocket_);
}

void ProxyServer::handleCommunication(boost::asio::ip::tcp::socket &sender,
                                      boost::asio::ip::tcp::socket &receiver)
{
    // Start asynchronous read operation on sender
    boost::asio::async_read_until(sender, buffer, '\n',
                                  [this, &sender, &receiver](const boost::system::error_code &readError, std::size_t bytesRead) {
                                      if (!readError) {
                                          std::istream is(&buffer);
                                          std::string message;
                                          std::getline(is, message);

                                          // Print the message in the proxy's terminal
                                          std::cout << "Proxy Server received message: " << message << std::endl;

                                          // Relay the message to the receiver
                                          relayMessage(receiver, message + "\n", [this, &sender, &receiver](const boost::system::error_code&) {
                                              // Continue handling communication
                                              handleCommunication(sender, receiver);
                                          });
                                      } else {
                                          // Handle the case where the sender socket is closed
                                          if (readError == boost::asio::error::eof) {
                                              std::cout << "Sender socket closed. Stopping communication.\n";
                                          } else {
                                              std::cerr << "Error while reading message: " << readError.message() << std::endl;
                                          }
                                      }
                                  });

    // Start asynchronous read operation on receiver
    boost::asio::async_read_until(receiver, buffer, '\n',
                                  [this, &receiver, &sender](const boost::system::error_code &readError, std::size_t bytesRead) {
                                      if (!readError) {
                                          std::istream is(&buffer);
                                          std::string message;
                                          std::getline(is, message);

                                          // Print the message in the proxy's terminal
                                          std::cout << "Proxy Server received message: " << message << std::endl;

                                          // Relay the message to the sender
                                          relayMessage(sender, message + "\n", [this, &receiver, &sender](const boost::system::error_code&) {
                                              // Continue handling communication
                                              handleCommunication(sender, receiver);
                                          });
                                      } else {
                                          // Handle the case where the receiver socket is closed
                                          if (readError == boost::asio::error::eof) {
                                              std::cout << "Receiver socket closed. Stopping communication.\n";
                                          } else {
                                              std::cerr << "Error while reading message: " << readError.message() << std::endl;
                                          }
                                      }
                                  });
}


void ProxyServer::relayMessage(boost::asio::ip::tcp::socket &receiver, const std::string &message, std::function<void(const boost::system::error_code&)> callback)
{
    // Capture the message variable
    std::string capturedMessage = message;

    // Relay the message to the receiver's terminal
    std::cout << "User received message: " << capturedMessage;

    // Relay the message to the receiver's socket
    boost::asio::async_write(receiver, boost::asio::buffer(capturedMessage),
                             [callback](const boost::system::error_code &writeError, std::size_t /*bytesWritten*/) {
                                 callback(writeError);
                             });
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
