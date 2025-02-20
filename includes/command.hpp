#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <map>
#include <string>
#include "server.hpp"

class command;
// Déclaration du type de la fonction
typedef void(command::*CommandFunction)(const std::string&);

class command
{
public:
    std::string userInput;
    std::map<std::string, CommandFunction> _cmds;
    Server& _server;
    void exec(const std::string &client_data);
    command(Server& server);
    void pass(const std::string &client_data);
    void nick(const std::string &client_data);
    // void user();
    // void join();
    // void part();
    // void privmsg();
    // void notice();
    // void ping();
    // void pong();
    // void quit();
    // void topic();
    // void mode();
    // void kick();
    // void invite();
    // void who();
    // void whois();
    // void whowas();
};

#endif // COMMAND_HPP