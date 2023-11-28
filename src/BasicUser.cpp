// BasicUser.cpp

#include "BasicUser.hpp"
#include <iostream>
#include <string>

BasicUser::BasicUser(const std::string &secret)
    : socket_(io_context_),
    secret_(secret)
{
}

void BasicUser::start()
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::asio::ip::tcp::resolver::query query("127.0.0.1", "12345");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    boost::asio::connect(socket_, endpoint_iterator);

    std::cout << "User connected to Proxy Server with secret: " << secret_ << std::endl;

    // Start a separate thread to handle user input asynchronously
    std::thread inputThread([this]() {
        while (true) {
            handleCommunication();
        }
    });

    // Run the IO context to handle asynchronous operations
    io_context_.run();

    // Wait for the input thread to finish
    inputThread.join();
}

void BasicUser::handleCommunication()
{
    // Allow the user to type the message
    std::string message;
    std::cout << "Type a message: ";
    std::getline(std::cin, message);

    // Send the message to the Proxy Server
    boost::asio::write(socket_, boost::asio::buffer(message + "\n"));
}
