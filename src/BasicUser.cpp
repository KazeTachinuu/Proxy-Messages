#include "BasicUser.hpp"

#include <future>

const std::string BasicUser::MSG_PREFIX = "[MSG]";
const std::string BasicUser::CMD_PREFIX = "[CMD]";
const std::string BasicUser::INFO_PREFIX = "[INFO]";
const std::string BasicUser::NEWLINE = "\n";

BasicUser::BasicUser(const std::string &channel)
    : socket_(io_context_)
    , disconnectTimer_(io_context_)
    , waitingTime_(30)
    , channel_(channel)
{}

void BasicUser::start()
{
    auto disconnectFuture = disconnectPromise_.get_future();

    connectToServer();
    handleDisconnect();

    // Start reading input in a separate thread
    std::thread inputThread([this, &disconnectFuture]() {
        // Wait for handleDisconnect to complete
        disconnectFuture.wait();

        std::cout << "Enter messages to send to the server. Type 'exit' to "
                     "quit.\n";
        readUserInput();
    });

    io_context_.run();

    // Wait for the input thread to finish
    inputThread.join();
}

void BasicUser::sendMessage(const std::string &message)
{
    boost::asio::write(socket_, boost::asio::buffer(message + NEWLINE));
}

void BasicUser::connectToServer()
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::asio::ip::tcp::resolver::query query("127.0.0.1", "12345");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
        resolver.resolve(query);

    boost::asio::connect(socket_, endpoint_iterator);

    sendMessage("[" + channel_ + "]");
}

void BasicUser::handleDisconnect()
{
    sendMessage(CMD_PREFIX + "GETUSERCOUNT");
    startReadUntilUserCount();
}

void BasicUser::startReadUntilUserCount()
{
    boost::asio::async_read_until(
        socket_, receiveBuffer_, '\n',
        [this](const boost::system::error_code &error,
               std::size_t bytes_received) {
            if (!error && bytes_received > 0)
            {
                std::istream is(&receiveBuffer_);
                std::string message;
                std::getline(is, message);

                std::cout << message << std::endl;

                if (message.find(INFO_PREFIX + "UserCount: ")
                    != std::string::npos)
                {
                    handleUserCountMessage(message);
                }
                else if (message.find(INFO_PREFIX + "New user connected:")
                         != std::string::npos)
                {
                    handleNewUserConnectedMessage();
                }
                else if (message.find(CMD_PREFIX + "SHUTDOWN")
                         != std::string::npos)
                {
                    handleShutdownMessage();
                }
                else
                {
                    startReadUntilUserCount();
                }
            }
        });
}

void BasicUser::handleUserCountMessage(const std::string &message)
{
    auto colonPos = message.find(':');
    int numConnectedUsers = std::stoi(message.substr(colonPos + 1));

    if (numConnectedUsers == 1)
    {
        handleAloneUser();
    }
    else
    {
        handleOtherUsersConnected();
    }
}

void BasicUser::handleNewUserConnectedMessage()
{
    std::cout << "Other users are connected. Stopping disconnect timer.\n";
    stopDisconnectTimer();
    disconnectPromise_.set_value();
    startRead();
}

void BasicUser::handleShutdownMessage()
{
    std::cout << "Server is shutting down. Disconnecting.\n";
    disconnectAndStop();
}

void BasicUser::handleAloneUser()
{
    std::cout << "You are alone. Waiting for " << waitingTime_ << " sec.\n";
    startDisconnectTimer();
    startReadUntilUserCount();
}

void BasicUser::handleOtherUsersConnected()
{
    std::cout << "Other users are connected.\n";
    stopDisconnectTimer();
    disconnectPromise_.set_value();
    startRead();
}

void BasicUser::readUserInput()
{
    std::string userInput;
    while (true)
    {
        // Read a line from the console
        std::getline(std::cin, userInput);

        if (userInput == "/help")
        {
            std::cout << "Commands:\n"
                         "/help - Print this help message\n"
                         "/exit - Exit the program\n"
                         "/usercount - Get the number of users connected\n"
                         "/ping <message> - Send a message to the server and "
                         "wait for an echo reply\n"
                         "/list - List all users in the channel\n\n";
            continue;
        }
        else if (userInput == "/exit")
        {
            break;
        }
        else if (userInput == "/list")
        {
            sendMessage(CMD_PREFIX + "GETUSERLIST");
            continue;
        }
        else if (userInput == "/usercount")
        {
            sendMessage(CMD_PREFIX + "GETUSERCOUNT");
            continue;
        }
        else if (userInput.find("/ping ") == 0)
        {
            std::size_t pingPos = userInput.find(' ');
            if (pingPos == std::string::npos)
            {
                std::cout << "No message to ping.\n\n";
                continue;
            }
            std::string msg = userInput.substr(pingPos + 1);
            sendMessage(CMD_PREFIX + "ECHOREPLY " + msg);
            continue;
        }

        // If no MSG or CMD prefix, add MSG
        if (userInput.find(MSG_PREFIX) != 0 && userInput.find(CMD_PREFIX) != 0)
        {
            userInput = MSG_PREFIX + userInput;
        }

        // Send the user input as a message to the server
        sendMessage(userInput);
    }

    // Stop the io_context when the input thread finishes
    io_context_.stop();
}

void BasicUser::startRead()
{
    boost::asio::async_read_until(socket_, receiveBuffer_, '\n',
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

        handleCommandResponse(message);

        startRead();
    }
}

void BasicUser::handleCommandResponse(const std::string &message)
{
    if (message.empty())
    {
        return;
    }
    if (message.find(INFO_PREFIX) != std::string::npos)
    {
        std::cout << message << std::endl << std::endl;
    }
    else if (message.find(MSG_PREFIX) != std::string::npos)
    {
        // Extract username and message body
        std::string username = message.substr(0, message.find(']') + 1);
        std::string messageBody = message.substr(message.find(MSG_PREFIX) + 5);
        std::cout << username << ": " << messageBody << std::endl;
    }
    else if (message.find(CMD_PREFIX + "ECHOREPLY") != std::string::npos)
    {
        // Echo reply received from the server
        std::cout << "Echo reply received: " << message << std::endl
                  << std::endl;
        size_t pos = message.find("ECHOREPLY");
        sendMessage(MSG_PREFIX + message.substr(pos + 11));
    }
    else if (message.find(CMD_PREFIX + "SHUTDOWN") != std::string::npos)
    {
        // Server is shutting down
        std::cout << "Server is shutting down. Disconnecting.\n";
        disconnectAndStop();
    }
    else
    {
        std::cout << "Unknown message: " << message << std::endl;
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
            disconnectAndStop();
        }
    });
}

void BasicUser::stopDisconnectTimer()
{
    disconnectTimer_.cancel();
}

void BasicUser::disconnectAndStop()
{
    socket_.close();

    io_context_.stop();

    exit(0);
}
