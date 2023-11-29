// BasicUser.cpp

#include "BasicUser.hpp"
#include <iostream>

BasicUser::BasicUser()
    : socket_(io_context_)
{
}

void BasicUser::start()
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::asio::ip::tcp::resolver::query query("127.0.0.1", "12345");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    boost::asio::connect(socket_, endpoint_iterator);


    // Start reading messages from the proxy server
    startRead();

    io_context_.run();
}

void BasicUser::startRead()
{
    boost::asio::async_read_until(
        socket_, receiveBuffer_, '\n',
        [this](const boost::system::error_code &error, std::size_t bytes_received) {
            if (!error && bytes_received > 0)
            {
                std::istream is(&receiveBuffer_);
                std::string message;
                std::getline(is, message);

                // Process the received message (print it for now)
                std::cout << "Received from Proxy Server:\n" << message << std::endl;

                // Continue reading messages
                startRead();
            }
        });
}
