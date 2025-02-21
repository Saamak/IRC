#include "includes/command.hpp"
#include <iostream>
#include "includes/colors.h"
#include <sstream>
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"

void command::pass(const std::string &client_data)
{
    std::cout << B_Y << "PASS Checking" << RESET << std::endl;
    std::cout << B_R << client_data << RESET << std::endl;

    std::istringstream iss(client_data);
    std::string command;
    std::string password;

    // Extraire le premier mot (la commande)
    iss >> command;
    // Extraire le deuxième mot (le mot de passe)
    iss >> password;
    if (!password.empty())
    {
        if(password != SECURE_PASSWD)
            P << B_R << "WRONG PASSWORD" <<RESET << E;
        else
        {
            std::cout <<B_G<<"PASSWORD VALIDATED" <<RESET << E;
            client* newClient = new client(); // Créer un nouvel objet client
            newClient->setMatch(true);
            _server.addClient(newClient); // Ajouter le client à la liste des clients du serveur
            _server.setNewClient(newClient); // Stocker temporairement le client dans le serveur
        }
    }
    else
        P << B_R << "Empty password" << RESET << E;
}

void command::nick(const std::string &client_data) {
    std::cout << "NICK" << std::endl;
    std::istringstream iss(client_data);
    std::string command;
    std::string nickname;

    // Extraire le premier mot (la commande)
    iss >> command;
    // Extraire le deuxième mot (le nickname)
    iss >> nickname;

    client* newClient = _server.getNewClient();
    if (newClient != NULL) {
        newClient->setNickname(nickname); // Définir le nickname pour le client
        std::cout << "Nickname set to: " << newClient->getNickname() << std::endl;
    } else {
        std::cerr << "Error: No client created. Please provide a valid password first." << std::endl;
    }
}

// void command::user(const std::string &client_data) {
//     std::cout << "USER" << std::endl;
// }

// void command::join(const std::string &client_data) {
//     std::cout << "JOIN" << std::endl;
//     channel* new_channel = new channel("dani"); // Crée un nouvel objet channel
//     _server.addChannel(new_channel); // Ajoute le nouvel objet channel au vecteur du serveur
//     P << "Channel created: " << new_channel->getName() << E;
// }

// void command::part() {
//     std::cout << "PART" << std::endl;
// }

// void command::privmsg() {
//     std::cout << "PRIVMSG" << std::endl;
// }

// void command::notice() {
//     std::cout << "NOTICE" << std::endl;
// }

// void command::ping() {
//     std::cout << "PING" << std::endl;
// }

// void command::pong() {
//     std::cout << "PONG" << std::endl;
// }

// void command::quit() {
//     std::cout << "QUIT" << std::endl;
// }

// void command::topic() {
//     std::cout << "TOPIC" << std::endl;
// }

// void command::mode() {
//     std::cout << "MODE" << std::endl;
// }

// void command::kick() {
//     std::cout << "KICK" << std::endl;
// }

// void command::invite() {
//     std::cout << "INVITE" << std::endl;
// }

// void command::who() {
//     std::cout << "WHO" << std::endl;
// }

// void command::whois() {
//     std::cout << "WHOIS" << std::endl;
// }

// void command::whowas() {
//     std::cout << "WHOWAS" << std::endl;
// }

command::command(Server& server) : _server(server)
{
    _cmds["PASS"] = &command::pass;
    _cmds["NICK"] = &command::nick;
    // _cmds["USER"] = &command::user;
    // _cmds["JOIN"] = &command::join;
    // _cmds["PART"] = &command::part;
    // _cmds["PRIVMSG"] = &command::privmsg;
    // _cmds["NOTICE"] = &command::notice;
    // _cmds["PING"] = &command::ping;
    // _cmds["PONG"] = &command::pong;
    // _cmds["QUIT"] = &command::quit;
    // _cmds["TOPIC"] = &command::topic;
    // _cmds["MODE"] = &command::mode;
    // _cmds["KICK"] = &command::kick;
    // _cmds["INVITE"] = &command::invite;
    // _cmds["WHO"] = &command::who;
    // _cmds["WHOIS"] = &command::whois;
    // _cmds["WHOWAS"] = &command::whowas;
}

void command::exec(const std::string &client_data) {
    std::stringstream ss(client_data);
    std::string buff;
    ss >> buff;
    std::map<std::string, CommandFunction>::iterator it = _cmds.find(buff);
    if (it != _cmds.end()) {
        (this->*(it->second))(client_data); // Appelle la fonction associée
    } else {
        std::cerr << "Command not found: " << client_data << std::endl;
    }
}