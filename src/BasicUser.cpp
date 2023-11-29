#include "BasicUser.hpp"
#include <iostream>
#include <chrono>

BasicUser::BasicUser()
    : socket_(io_context_),
        disconnectTimer_(io_context_)

{
}

void BasicUser::start()
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::asio::ip::tcp::resolver::query query("127.0.0.1", "12345");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    boost::asio::connect(socket_, endpoint_iterator);

    // Start reading messages from the server
    startRead();

    // Start a timer to disconnect if no one else connects within 30 seconds
    startDisconnectTimer();

    // Run the IO context
    io_context_.run();
}

void BasicUser::startRead()
{
    boost::asio::async_read_until(
        socket_, receiveBuffer_, '\n',
        [this](const boost::system::error_code& error, std::size_t bytes_received) {
            if (!error && bytes_received > 0)
            {
                std::istream is(&receiveBuffer_);
                std::string message;
                std::getline(is, message);

                std::cout << "Server:\n" << message << std::endl;

                // Check if the received message is "User connected."
                if (message == "User connected.")
                {
                    // Trigger the sendMessage function with a response message
                    sendMessage("Hello, server! I received the 'User connected' message.");
                }

                // Continue reading messages
                startRead();
            }
        });
}

void BasicUser::sendMessage(const std::string& message)
{
    boost::asio::write(socket_, boost::asio::buffer(message + "\n"));
}

void BasicUser::startDisconnectTimer()
{
    disconnectTimer_.expires_from_now(boost::posix_time::seconds(30));
    disconnectTimer_.async_wait(
        [this](const boost::system::error_code& error) {
            if (!error)
            {
                std::cout << "No one else connected within 30 seconds. Disconnecting.\n";
                socket_.close();
                io_context_.stop();
            }
        });
}
