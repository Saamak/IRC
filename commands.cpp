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
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    if (password.empty()) {
        // Utilisation de sendIrcMessage pour ERR_NEEDMOREPARAMS
        _server.sendIrcMessage(_server.getServerName(), "ERR_NEEDMOREPARAMS", "", "", "Not enough parameters", fd);
        exec("QUIT");
        return;
    }

    if (Client_tmp[_server.getIterator() - 1]->getRegistered()) {
        // Utilisation de sendIrcMessage pour ERR_ALREADYREGISTRED
        _server.sendIrcMessage(_server.getServerName(), "ERR_ALREADYREGISTRED", "", "", "You are already registered", fd);
        return;
    }

    if (!Client_tmp[_server.getIterator() - 1]->getRegistered() &&
        (Client_tmp[_server.getIterator() - 1]->getNickCheck() || Client_tmp[_server.getIterator() - 1]->getUserCheck())) {
        // Utilisation de sendIrcMessage pour une erreur personnalisée
        _server.sendIrcMessage(_server.getServerName(), "ERR_BADCOMMANDORDER", "", "", "Commands bad order during connection, PASS/NICK/USER. Client Disconnected", fd);
        exec("QUIT");
        return;
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
    if (nickname.empty()) {
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        int fd = pollfd_tmp[_server.getIterator()].fd;
        _server.sendIrcMessage(_server.getServerName(), "ERR_NEEDMOREPARAMS", "", "", "Not enough parameters", fd);
        exec("QUIT");
        return;
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
            std::string message = "Client Connected in\n" + _server.getServerName();
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

    iss >> command;
    iss >> channel_name;

    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    std::vector<client*>& Client_tmp = _server.getClientList();
    std::string nickname = Client_tmp[_server.getIterator() - 1]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    bool channelExists = false;
    for (size_t x = 0; x < Channel_tmp.size(); x++) {
        if (Channel_tmp[x]->getName() == channel_name) {
            channelExists = true;
            if (!Channel_tmp[x]->IsInChannel(nickname)) {
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
        P << "Channel created and client added" << B_R << "(Operator) " << RESET << new_channel->getName() << E;
    }

    // Send JOIN message
    _server.sendIrcMessage(_server.getServerName(), "JOIN", nickname, channel_name, "", fd);

    // Send RPL_TOPIC (332) message
    std::string topic = "Welcome to the channel!";
    _server.sendIrcMessage(_server.getServerName(), "RPL_TOPIC", nickname, channel_name, topic, fd);

    // Send RPL_NAMREPLY (353) message
    std::string names_list = nickname; // Add other users in the channel if needed
    _server.sendIrcMessage(_server.getServerName(), "RPL_NAMREPLY", nickname, channel_name, names_list, fd);

    // Send RPL_ENDOFNAMES (366) message
    _server.sendIrcMessage(_server.getServerName(), "RPL_ENDOFNAMES", nickname, channel_name, "", fd);

    _server.printChannelsAndClients();
}

void command::mode(const std::string &client_data) {
    std::istringstream iss(client_data);
    std::string command;
    std::string channel_name;

    iss >> command;
    iss >> channel_name;

    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    std::vector<client*>& Client_tmp = _server.getClientList();
    std::string nickname = Client_tmp[_server.getIterator() - 1]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    for (size_t x = 0; x < Channel_tmp.size(); x++) {
        if (Channel_tmp[x]->getName() == channel_name) {
            _server.sendIrcMessage(_server.getServerName(), "RPL_CHANNELMODEIS", nickname, channel_name, "+nt", fd);
            return;
        }
    }

    _server.sendIrcMessage(_server.getServerName(), "ERR_NOSUCHCHANNEL", nickname, channel_name, "", fd);
}

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

void command::topic(const std::string &client_data) {
    std::istringstream iss(client_data);
    std::string command;
    std::string channel_name;
    std::string topic_name;

    iss >> command;
    iss >> channel_name;
    std::getline(iss, topic_name);

    if (!topic_name.empty() && topic_name[0] == ' ') {
        topic_name.erase(0, 1);
    }

    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    std::vector<client*>& Client_tmp = _server.getClientList();
    std::string nickname = Client_tmp[_server.getIterator() - 1]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    for (size_t x = 0; x < Channel_tmp.size(); x++) {
        if (Channel_tmp[x]->getName() == channel_name) {
            if (topic_name.empty()) {
                if (Channel_tmp[x]->getTopic().empty()) {
                    _server.sendIrcMessage(_server.getServerName(), "ERR_NOTOPIC", nickname, channel_name, "No topic is set.", fd);
                } else {
                    _server.sendIrcMessage(_server.getServerName(), "RPL_TOPIC", nickname, channel_name, Channel_tmp[x]->getTopic(), fd);
                }
            } else {
                Channel_tmp[x]->setTopic(topic_name);
                _server.sendIrcMessage(_server.getServerName(), "TOPIC", nickname, channel_name, topic_name, fd);
                P << B_Y << "Channel topic changed to: " << B_R << Channel_tmp[x]->getTopic() << RESET << E;
            }
            return;
        }
    }

    _server.sendIrcMessage(_server.getServerName(), "ERR_NOSUCHCHANNEL", nickname, channel_name, "", fd);
}

void command::myExit(const std::string &client_data)
{
    (void)client_data;
    delete(_server.getNewClient());
    _server.setNewClient(NULL); // Réinitialiser le pointeur
    _server.setBoolExit(true);
}

void    command::cap(const std::string &client_data)
{
    (void)client_data;
    return ;   
}

command::command(Server& server) : _server(server)
{
    _cmds["PASS"] = &command::pass;
    _cmds["NICK"] = &command::nick;
    _cmds["USER"] = &command::user;
    _cmds["JOIN"] = &command::join;
    _cmds["CAP"] = &command::cap;
    // _cmds["PART"] = &command::part;
    // _cmds["PRIVMSG"] = &command::privmsg;
    // _cmds["NOTICE"] = &command::notice;
    _cmds["QUIT"] = &command::quit;
    _cmds["TOPIC"] = &command::topic;
    _cmds["MODE"] = &command::mode;
    // _cmds["KICK"] = &command::kick;
    // _cmds["INVITE"] = &command::invite;
    // _cmds["WHO"] = &command::who;s
    // _cmds["WHOIS"] = &command::whois;
    // _cmds["WHOWAS"] = &command::whowas;

    _cmds["MYEXIT"] = &command::myExit;
}

void command::exec(const std::string &client_data) {
    std::stringstream iss(client_data);
    std::string buff;
    iss >> buff;
    std::map<std::string, CommandFunction>::iterator it = _cmds.find(buff);
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
        std::string message = "Command not found\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
    }
}