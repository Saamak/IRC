#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <map>
#include <string>

// Déclaration du type de la fonction
typedef void(*CommandFunction)(void);

class command {
public:
    std::map<std::string, CommandFunction> _cmds;
    void exec(const std::string &client_data);
    command();
    static void nick();
    static void user();
    static void join();
    static void part();
    static void privmsg();
    static void notice();
    static void ping();
    static void pong();
    static void quit();
    static void topic();
    static void mode();
    static void kick();
    static void invite();
    static void who();
    static void whois();
    static void whowas();
};

#endif // COMMAND_HPP