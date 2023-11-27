// ProxyServer.cpp
#include "ProxyServer.hpp"
#include <iostream>
#include <algorithm>
#include <boost/asio/io_service.hpp>

using boost::asio::ip::tcp;

ProxyServer::ProxyServer(const std::string &secret)
    : secret(secret),
    io_service(),
    acceptor(io_service, tcp::endpoint(tcp::v4(), 12345)), // Listening on port 12345
    socket(io_service)
{
    handleAccept(); // Start the initial accept
}


void ProxyServer::start()
{
    io_service.run();
}

void ProxyServer::handleAccept()
{
    std::shared_ptr<tcp::socket> newSocket = std::make_shared<tcp::socket>(io_service);

    acceptor.async_accept(*newSocket, [this, newSocket](const boost::system::error_code &ec) {
        if (!ec)
        {
            std::cout << "Connection accepted from a user.\n";

            // Add the new socket to the vector
            sockets.push_back(newSocket);

            // Handle communication logic here
            startReading(newSocket);

            // Continue waiting for the next connection
            handleAccept();
        }
        else
        {
            std::cerr << "Error accepting connection: " << ec.message() << std::endl;
        }
    });
}

void ProxyServer::startReading(std::shared_ptr<tcp::socket> socket)
{
    boost::asio::async_read_until(*socket, buffer, '\n', [this, socket](const boost::system::error_code &error, std::size_t bytes_transferred) {
        if (!error)
        {
            std::istream is(&buffer);
            std::string message;
            std::getline(is, message);

            // Print the received message
            std::cout << "Received message: " << message << std::endl;

            // Forward the message to other connected users
            startWriting(socket, message);

            // Continue reading from the socket
            startReading(socket);
        }
        else
        {
            std::cerr << "Error reading from socket: " << error.message() << std::endl;

            // Remove the socket from the vector
            sockets.erase(std::remove(sockets.begin(), sockets.end(), socket), sockets.end());
        }
    });
}

void ProxyServer::startWriting(std::shared_ptr<tcp::socket> senderSocket, const std::string &message)
{
    // Iterate through connected sockets and send the message
    for (const auto &receiverSocket : sockets)
    {
        if (receiverSocket != senderSocket) // Don't send to the sender
        {
            boost::asio::async_write(*receiverSocket, boost::asio::buffer(message + "\n"), [this](const boost::system::error_code &error, std::size_t bytes_transferred) {
                if (error)
                {
                    std::cerr << "Error writing to socket: " << error.message() << std::endl;
                }
            });
        }
    }

    // Clear the buffer after writing
    buffer.consume(buffer.size());
}
