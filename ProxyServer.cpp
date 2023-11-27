#include "ProxyServer.hpp"

#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

ProxyServer::ProxyServer(const std::string &secret)
    : secret(secret)
{}

void ProxyServer::start()
{
    try
    {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(
            io_service,
            tcp::endpoint(tcp::v4(), 12345)); // Listening on port 12345

        while (true)
        {
            tcp::socket socket(io_service);
            acceptor.accept(socket);

            // Perform communication logic here
            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');
            std::istream is(&buffer);
            std::string message;
            std::getline(is, message);

            std::cout << "Proxy Server received: " << message << std::endl;

            // You can continue reading/writing from/to the socket as needed

            // Close the socket when done
            socket.close();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Proxy Server Exception: " << e.what() << std::endl;
    }
}