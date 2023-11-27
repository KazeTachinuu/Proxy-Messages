// BasicUser.cpp
#include "BasicUser.hpp"

#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

BasicUser::BasicUser(const std::string &secret)
    : secret(secret)
{}

void BasicUser::start()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(
            "localhost",
            "12345"); // Assuming server is running on localhost, port 12345
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        // Perform communication logic here
        std::string message = "Hello from Basic User!";
        boost::asio::write(socket, boost::asio::buffer(message));

        // You can continue reading/writing from/to the socket as needed

        // Close the socket when done
        socket.close();
    }
    catch (std::exception &e)
    {
        std::cerr << "Basic User Exception: " << e.what() << std::endl;
    }
}
