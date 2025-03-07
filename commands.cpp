#include "includes/command.hpp"
#include <iostream>
#include "includes/colors.h"
#include <sstream>
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"
#include <cstdlib> 

command::~command(){
    P <<BOLD<< "COMMAND DESTRUCTEUR" <<RESET<< E;
}

void command::pass(const std::string &client_data)
{
    // std::cout << B_Y << "PASS Checking" << RESET << std::endl;
    // std::cout << B_R << client_data << RESET << std::endl;

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
            //delete(newClient);
        }
    }
    else
        P << B_R << "Empty password" << RESET << E;
}

void command::nick(const std::string &client_data) {
    // std::cout << "NICK" << std::endl;
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
        std::cout << "Client Nickname set to: " << newClient->getNickname() << std::endl;
    } else {
        std::cerr << "Error: No client created. Please provide a valid password first." << std::endl;
    }
}

void command::user(const std::string &client_data) 
{
    std::istringstream iss(client_data);
    std::string command;
    std::string username;
    iss >> command;
    iss >> username;
    client* newClient = _server.getNewClient();
    newClient->setUsername(username);
}

void command::join(const std::string &client_data) {
    std::istringstream iss(client_data);
    std::string command;
    std::string channel_name;

    // Extraire le premier mot (la commande)
    iss >> command;
    // Extraire le deuxième mot (le channel_name)
    iss >> channel_name;

    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    std::vector<client*>& Client_tmp = _server.getClientList();
    bool channelExists = false;
    for (size_t x = 0; x < Channel_tmp.size(); x++) {
        if (Channel_tmp[x]->getName() == channel_name) {
            channelExists = true;
            if (!Channel_tmp[x]->IsInChannel(Client_tmp[_server.getIterator() - 1]->getNickname())) {
                Channel_tmp[x]->addClient(Client_tmp[_server.getIterator() - 1]);
                P << "Client added to existing channel" << E;
            }
            break;
        }
    }
    if (!channelExists) {
        channel* new_channel = new channel(channel_name);
        new_channel->addClient(Client_tmp[_server.getIterator() - 1]);
        new_channel->addOperator(Client_tmp[_server.getIterator() - 1]);
        _server.addChannel(new_channel);
        P << "Channel created and client added"<<B_R<< "(Operator) " <<RESET<< new_channel->getName()<<E;
    }
    _server.printChannelsAndClients();
}

// void command::part() {
//     std::cout << "PART" << std::endl;
// }

// void command::privmsg() {
//     std::cout << "PRIVMSG" << std::endl;
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

void command::myExit(const std::string &client_data)
{
    (void)client_data;
    delete(_server.getNewClient());
    _server.setNewClient(NULL); // Réinitialiser le pointeur
    _server.setBoolExit(true);
}

command::command(Server& server) : _server(server)
{
    _cmds["PASS"] = &command::pass;
    _cmds["NICK"] = &command::nick;
    _cmds["USER"] = &command::user;
    _cmds["JOIN"] = &command::join;
    // _cmds["PART"] = &command::part;
    // _cmds["PRIVMSG"] = &command::privmsg;
    // _cmds["NOTICE"] = &command::notice;
    // _cmds["QUIT"] = &command::quit;
    // _cmds["TOPIC"] = &command::topic;
    // _cmds["MODE"] = &command::mode;
    // _cmds["KICK"] = &command::kick;
    // _cmds["INVITE"] = &command::invite;
    // _cmds["WHO"] = &command::who;
    // _cmds["WHOIS"] = &command::whois;
    // _cmds["WHOWAS"] = &command::whowas;

    _cmds["MYEXIT"] = &command::myExit;
}

void command::exec(const std::string &client_data) {
    std::stringstream iss(client_data);
    std::string buff;
    iss >> buff;
    std::map<std::string, CommandFunction>::iterator it = _cmds.find(buff);
    if (it != _cmds.end()) {
        (this->*(it->second))(client_data); // Appelle la fonction associée
    } else {
        std::cerr << "Command not found: " << client_data << std::endl;
    }
}