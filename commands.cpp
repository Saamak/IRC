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

void command::pass(const std::string &client_data) // Pass daniel
{
    std::istringstream iss(client_data);
    std::string command;
    std::string password;
    iss >> command;
    iss >> password;
    std::vector<client*>& Client_tmp = _server.getClientList();
    if (password.empty())
    {
        std::string message = "ERR_NEEDMOREPARAMS\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        exec("QUIT");
        return ;
    }
    if(Client_tmp[_server.getIterator() -1]->getRegistered())
    {
        std::string message = "ERR_ALREADYREGISTRED\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        return ;
    }
    if (Client_tmp[_server.getIterator() -1]->getRegistered() == false && (Client_tmp[_server.getIterator() -1]->getNickCheck() == true || Client_tmp[_server.getIterator() -1]->getUserCheck() == true))
    {
        std::string message = "Commands bad order during connection, PASS/NICK/USER\nClient Disconnected\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        exec("QUIT");
        return ;
    }
    Client_tmp[_server.getIterator() -1]->setPassCheck(true);
    Client_tmp[_server.getIterator() -1]->setClientPassword(password);
}

void command::nick(const std::string &client_data) {

    std::vector<client*>& Client_tmp = _server.getClientList();
    std::istringstream iss(client_data);
    std::string command;
    std::string nickname;
    iss >> command;
    iss >> nickname;
    P << nickname << E;
    if(nickname.empty())
    {
        std::string message = "ERR_NEEDMOREPARAMS\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        exec("QUIT");
        return ;
    }
    if(Client_tmp[_server.getIterator() -1]->getRegistered())
    {
        //CHECK SI CE NICK EST DEJA REGISTERED SUR LE SERV, SI OUI , REFUSER LE NOUVEAU NICK, DONC LAISSER L'ACTUEl
        Client_tmp[_server.getIterator() -1]->setNickname(nickname);
        std::string message = "You are now known as " + nickname + "\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        return ;
    }
    if(Client_tmp[_server.getIterator() -1]->getPassCheck() && Client_tmp[_server.getIterator() -1]->getUserCheck() == false && Client_tmp[_server.getIterator() -1]->getNickCheck() == false)
    {
        //CHECK SI CE NICK EST DEJA REGISTERED SUR LE SERV, SI OUI , REFUSER LA CONNECTION
        Client_tmp[_server.getIterator() -1]->setNickCheck(true);
        Client_tmp[_server.getIterator() -1]->setNickname(nickname);
        return ;
    }
    if(Client_tmp[_server.getIterator() -1]->getPassCheck() == false || Client_tmp[_server.getIterator() -1]->getNickCheck())
    {
        std::string message = "Commands bad order during connection, PASS/NICK/USER\nClient Disconnected\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        exec("QUIT");
        return ;
    }
}

void command::user(const std::string &client_data) 
{
    std::vector<client*>& Client_tmp = _server.getClientList();
    std::istringstream iss(client_data);
    std::string command;
    std::string username;
    iss >> command;
    iss >> username;
    if (username.empty())
    {
        std::string message = "ERR_NEEDMOREPARAMS\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        exec("QUIT");
        return ;
    }
    if(Client_tmp[_server.getIterator() -1]->getRegistered())
    {
        std::string message = "ERR_ALREADYREGISTRED\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        return ;
    }
    if (Client_tmp[_server.getIterator() -1]->getPassCheck() == false || Client_tmp[_server.getIterator() -1]->getNickCheck() == false)
    {
        std::string message = "Commands bad order during connection, PASS/NICK/USER\nClient Disconnected\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
        exec("QUIT");
        return ;
    }
    if (Client_tmp[_server.getIterator() -1]->getPassCheck() && Client_tmp[_server.getIterator() -1]->getNickCheck())
    {
        Client_tmp[_server.getIterator() -1]->setUserCheck(true);
        Client_tmp[_server.getIterator() -1]->setUsername(username);
        if (Client_tmp[_server.getIterator() -1]->getClientPassword() == _server.getPassword())
        {
            std::string message = "Client Connected in German Elite V2\n";
            std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
            send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
            Client_tmp[_server.getIterator() -1]->setRegistered(true);
        }   
        else
        {
            std::string message = "Wrong Password\nClient Disconnected\n";
            std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
            send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
            exec("QUIT");
        }
        return ;
    }
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

void command::quit(const std::string &client_data)
{
    (void)client_data;
    std::vector<client*>& Client_tmp = _server.getClientList();
    std::vector<struct pollfd>& Poll_tmp = _server.getPollFd();

    Client_tmp.erase (Client_tmp.begin() + _server.getIterator() - 1);
    Poll_tmp.erase(Poll_tmp.begin() + _server.getIterator());
    _server.setIterator(_server.getIterator() - 1);

    std::cout << "QUIT" << std::endl;
}

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
    _cmds["QUIT"] = &command::quit;
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
    P << buff << E;
    if (it != _cmds.end()) 
    {
        (this->*(it->second))(client_data); // Appelle la fonction associée
    } 
    else 
    {
        std::vector<client*>& Client_tmp = _server.getClientList();
        if (Client_tmp[_server.getIterator() -1]->getRegistered() == false)
        {
            std::string message = "Command not found during connection PASS/NICK/USER\nClient Disconnected\n";
            std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
            send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
            exec("QUIT");
        }
        std::cerr << "Command not found: " << client_data << std::endl;
    }
}