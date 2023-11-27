// BasicUser.cpp
#include "BasicUser.hpp"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

BasicUser::BasicUser(const std::string &secret) : secret(secret) {}

void BasicUser::start()
{
    try
    {
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query("localhost", "12345"); // Assuming server is running on localhost, port 12345
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        // Start a separate thread for user input
        std::thread userInputThread([this, &socket] { startUserInput(socket); });

        // Perform communication logic here
        startReading(socket);

        // Wait for the user input thread to finish
        userInputThread.join();
    }
    catch (std::exception &e)
    {
        std::cerr << "Basic User Exception: " << e.what() << std::endl;
    }
}

void BasicUser::startReading(tcp::socket &socket)
{
    boost::asio::streambuf buffer;
    boost::asio::async_read_until(socket, buffer, '\n', [this, &socket, &buffer](const boost::system::error_code &error, std::size_t bytes_transferred) {
        if (!error)
        {
            std::istream is(&buffer);
            std::string message;
            std::getline(is, message);

            // Print the received message
            std::cout << "Received message: " << message << std::endl;

            // Continue reading from the socket
            startReading(socket);
        }
        else
        {
            std::cerr << "Error reading from socket: " << error.message() << std::endl;
        }
    });
}

void BasicUser::startUserInput(tcp::socket &socket)
{
    try
    {
        while (true)
        {
            std::string message;
            std::cout << "Type a message: ";
            std::getline(std::cin, message);

            // Send the message to the proxy server
            boost::asio::write(socket, boost::asio::buffer(message + "\n"));
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "User Input Exception: " << e.what() << std::endl;
    }
}
