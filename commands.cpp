#include "command.hpp"
#include <iostream>
#include "colors.h"
#include <sstream>

void command::kick() {
    std::cout << "KICK" << std::endl;
}

void command::invite() {
    std::cout << "INVITE" << std::endl;
}

void command::topic() {
    std::cout << "TOPIC" << std::endl;
}

void command::mode() {
    std::cout << "MODE" << std::endl;
}

command::command() {
    _cmds["KICK"] = &command::kick;
    _cmds["INVITE"] = &command::invite;
    _cmds["TOPIC"] = &command::topic;
    _cmds["MODE"] = &command::mode;
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