#include <algorithm>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <iostream>

#include "BasicUser.hpp"

namespace asio = boost::asio;

BasicUser::BasicUser(const std::string &channel)
    : socket_(io_context_)
    , disconnectTimer_(io_context_)
    , waitingTime_(30)
    , channel_(channel)
{}

void BasicUser::start()
{
    asio::ip::tcp::resolver resolver(io_context_);
    asio::ip::tcp::resolver::query query("127.0.0.1", "12345");
    asio::ip::tcp::resolver::iterator endpoint_iterator =
        resolver.resolve(query);

    asio::connect(socket_, endpoint_iterator);

    handleDisconnect();

    // Run the IO context
    io_context_.run();
}

void BasicUser::startRead()
{
    asio::async_read_until(socket_, receiveBuffer_, '\n',
                           [this](const boost::system::error_code &error,
                                  std::size_t bytes_received) {
                               handleRead(error, bytes_received);
                           });
}

void BasicUser::handleRead(const boost::system::error_code &error,
                           std::size_t bytes_received)
{
    if (!error && bytes_received > 0)
    {
        std::istream is(&receiveBuffer_);
        std::string message;
        std::getline(is, message);

        std::cout << "Server: " << message << std::endl;

        // Handle responses for various commands
        handleCommandResponse(message);

        // Continue reading messages
        startRead();
    }
}

void BasicUser::handleCommandResponse(const std::string &message)
{
    if (message.find("[INFO]UserCount") == 0)
    {
        std::cout << message << std::endl;
    }
    if (message.find("[CMD]ECHOREPLY") == 0)
    {
        size_t pos = message.find("ECHOREPLY");
        sendMessage("[MSG]" + message.substr(pos + 11));
    }
}

void BasicUser::startDisconnectTimer()
{
    disconnectTimer_.expires_from_now(boost::posix_time::seconds(waitingTime_));
    disconnectTimer_.async_wait([this](const boost::system::error_code &error) {
        if (!error)
        {
            std::cout << "No one else connected within " << waitingTime_
                      << " seconds. Disconnecting.\n";
            socket_.close();
            io_context_.stop();
        }
    });
}

void BasicUser::stopDisconnectTimer()
{
    disconnectTimer_.cancel();
}

void BasicUser::handleDisconnect()
{
    sendMessage("[CMD]GetUserCount");
    startReadUntilUserCount();
}

void BasicUser::startReadUntilUserCount()
{
    asio::async_read_until(
        socket_, receiveBuffer_, '\n',
        [this](const boost::system::error_code &error,
               std::size_t bytes_received) {
            if (!error && bytes_received > 0)
            {
                std::istream is(&receiveBuffer_);
                std::string message;
                std::getline(is, message);

                std::cout << message << std::endl;

                if (message.find("[INFO]UserCount: ") == 0)
                {
                    auto colonPos = message.find(':');
                    int numConnectedUsers =
                        std::stoi(message.substr(colonPos + 1));
                    if (numConnectedUsers == 1)
                    {
                        std::cout << "You are alone. Waiting for "
                                  << waitingTime_ << " sec.\n";
                        startDisconnectTimer();
                        startReadUntilUserCount();
                    }
                    else
                    {
                        std::cout << "Other users are connected.\n";
                        stopDisconnectTimer();
                        startRead();
                    }
                }
                else if (message.find("[INFO]New user connected:") == 0)
                {
                    std::cout << "Other users are connected. Stopping "
                                 "disconnect timer.\n";
                    stopDisconnectTimer();
                    startRead();
                }
                else
                {
                    startReadUntilUserCount();
                }
            }
        });
}

void BasicUser::sendMessage(const std::string &message)
{
    asio::write(socket_, asio::buffer(message + "\n"));
}
