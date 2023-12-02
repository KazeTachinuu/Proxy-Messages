#ifndef BASIC_USER_HPP
#define BASIC_USER_HPP

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <future>
#include <iostream>
#include <thread>

class BasicUser
{
public:
    BasicUser(const std::string &channel);

    void start();
    void sendMessage(const std::string &message);

private:
    static const std::string MSG_PREFIX;
    static const std::string CMD_PREFIX;
    static const std::string INFO_PREFIX;
    static const std::string NEWLINE;

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf receiveBuffer_;
    boost::asio::deadline_timer disconnectTimer_;
    std::promise<void> disconnectPromise_;
    int waitingTime_;
    std::string channel_;

    void readUserInput();

    void connectToServer();
    void startRead();
    void handleRead(const boost::system::error_code &error,
                    std::size_t bytes_received);
    void handleCommandResponse(const std::string &message);
    void startDisconnectTimer();
    void stopDisconnectTimer();
    void handleDisconnect();
    void startReadUntilUserCount();
    std::string extractUserMessage(const std::string &message) const;
    void disconnectAndStop();

    // Refactored functions
    void handleUserCountMessage(const std::string &message);
    void handleNewUserConnectedMessage();
    void handleShutdownMessage();
    void handleAloneUser();
    void handleOtherUsersConnected();
};

#endif // BASIC_USER_HPP
