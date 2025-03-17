#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <map>
#include <string>
#include "server.hpp"
#include "client.hpp"

class command;
// Déclaration du type de la fonction
typedef void(command::*CommandFunction)(const std::string&);

class command
{
public:
    ~command();
    std::string userInput;
    std::map<std::string, CommandFunction> _cmds;
    Server& _server;
    void exec(const std::string &client_data);
    command(Server& server);
    void pass(const std::string &client_data);
    void nick(const std::string &client_data);
    void myExit(const std::string &client_data);
    void user(const std::string &client_data);
    void join(const std::string &client_data);
    void cap(const std::string &client_data);
    // void part();
    // void privmsg();
    // void notice();
    void quit(const std::string &client_data);
    void topic(const std::string &client_data);
    void mode(const std::string &client_data);
    // void kick();
    // void invite();
    // void who();
    // void whois();
    // void whowas();
};

#endif // COMMAND_HPP