// BasicUser.cpp

#include "BasicUser.hpp"

#include <iostream>

BasicUser::BasicUser()
    : socket_(io_context_)
{}

void BasicUser::start()
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::asio::ip::tcp::resolver::query query("127.0.0.1", "12345");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
        resolver.resolve(query);

    boost::asio::connect(socket_, endpoint_iterator);

    std::cout << "User connected to Proxy Server.\n";

    // Start asynchronous message receiving
    asyncReceive();

    io_context_.run();
}

void BasicUser::asyncReceive()
{
    socket_.async_receive(
        boost::asio::buffer(receiveBuffer_),
        [this](const boost::system::error_code &error,
               std::size_t bytes_received) {
            if (!error)
            {
                std::cout << "Received message from Proxy:\n"
                          << std::string(receiveBuffer_.data(), bytes_received)
                          << "\n";

                // Continue listening for messages
                asyncReceive();
            }
            else
            {
                std::cerr << "Error receiving message: " << error.message()
                          << "\n";
            }
        });
}
