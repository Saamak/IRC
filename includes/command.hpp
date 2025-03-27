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
private:
    size_t iterator_mode;
public:
    ~command();
    std::string userInput;
    std::map<std::string, CommandFunction> _cmds;
    Server& _server;
    void    exec(const std::string &client_data);
    command(Server& server);
    void    pass(const std::string &client_data);
    void    nick(const std::string &client_data);
    void    user(const std::string &client_data);
    void    join(const std::string &client_data);
    void    cap(const std::string &client_data);
    size_t getChanIterator(std::string channelname);
    // void part();
    void    privmsg(const std::string &client_data);
    void    invite(const std::string &client_data);
    void    processChannelMessage(const std::string& targetChannel, const std::string& message, const std::string& nickname, int sender_fd);
    void    processPrivateMessage(const std::string& targetChannel, const std::string& message, const std::string& nickname, int sender_fd);
    // void notice();
    void    quit(const std::string &client_data);
    void    topic(const std::string &client_data);
    void    mode(const std::string &client_data);
    void    kick(const std::string &client_data);
    void    removeUserCommand(const std::string &nickname, const std::string &channelName);
    // void invite();
    // void who();
    // void whois();
    // void whowas();

    void sendIt(std::string def, int fdClient);
    std::vector<std::pair<std::string,std::string> > parsing_param_mode(const std::string &client_data);
    std::pair<std::string,std::string> get_mode_and_sign(std::string flag);
    std::string get_previous_sign(std::string flag);
};

#endif // COMMAND_HPP