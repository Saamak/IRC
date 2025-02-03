#include "command.hpp"
#include <iostream>
#include "colors.h"
#include <sstream>

void command::nick() {
    std::cout << "NICK" << std::endl;
}

void command::user() {
    std::cout << "USER" << std::endl;
}

void command::join() {
    std::cout << "JOIN" << std::endl;
}

void command::part() {
    std::cout << "PART" << std::endl;
}

void command::privmsg() {
    std::cout << "PRIVMSG" << std::endl;
}

void command::notice() {
    std::cout << "NOTICE" << std::endl;
}

void command::ping() {
    std::cout << "PING" << std::endl;
}

void command::pong() {
    std::cout << "PONG" << std::endl;
}

void command::quit() {
    std::cout << "QUIT" << std::endl;
}

void command::topic() {
    std::cout << "TOPIC" << std::endl;
}

void command::mode() {
    std::cout << "MODE" << std::endl;
}

void command::kick() {
    std::cout << "KICK" << std::endl;
}

void command::invite() {
    std::cout << "INVITE" << std::endl;
}

void command::who() {
    std::cout << "WHO" << std::endl;
}

void command::whois() {
    std::cout << "WHOIS" << std::endl;
}

void command::whowas() {
    std::cout << "WHOWAS" << std::endl;
}

command::command() {
    _cmds["NICK"] = &command::nick;
    _cmds["USER"] = &command::user;
    _cmds["JOIN"] = &command::join;
    _cmds["PART"] = &command::part;
    _cmds["PRIVMSG"] = &command::privmsg;
    _cmds["NOTICE"] = &command::notice;
    _cmds["PING"] = &command::ping;
    _cmds["PONG"] = &command::pong;
    _cmds["QUIT"] = &command::quit;
    _cmds["TOPIC"] = &command::topic;
    _cmds["MODE"] = &command::mode;
    _cmds["KICK"] = &command::kick;
    _cmds["INVITE"] = &command::invite;
    _cmds["WHO"] = &command::who;
    _cmds["WHOIS"] = &command::whois;
    _cmds["WHOWAS"] = &command::whowas;
}

void command::exec(const std::string &client_data) {
    std::stringstream ss(client_data);
    std::string buff;
    ss >> buff;
    std::map<std::string, CommandFunction>::iterator it = _cmds.find(buff);
    if (it != _cmds.end()) {
        it->second(); // Appelle la fonction associée
    } else {
        std::cerr << "Command not found: " << client_data << std::endl;
    }
}