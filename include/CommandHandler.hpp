#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <boost/asio.hpp>
#include <functional>
#include <map>
#include <memory>

class ProxyServer;

class CommandHandler
{
public:
    using CommandFunction = std::function<void(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &,
        const std::string &, const std::string &)>;

    CommandHandler(ProxyServer &proxyServer);

    void handleCommand(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
        const std::string &command, const std::string &channel);

private:
    void registerCommand(const std::string &command, CommandFunction handler);

    // Command handlers
    void handleGetUserConnected(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
        const std::string &command, const std::string &channel);

    void handleEchoReply(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
        const std::string &command, const std::string &channel);

    void handleGetUserList(
        const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
        const std::string &command, const std::string &channel);

private:
    ProxyServer &proxyServer_;
    std::map<std::string, CommandFunction> commandHandlers_;
};

#endif // COMMANDHANDLER_HPP
