#include "includes/command.hpp"
#include <iostream>
#include "includes/colors.h"
#include <sstream>
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"
#include <cstdlib> 
#include <utility>

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
    size_t iterator = _server.getIterator() - 1;

    if (password.empty()) {
        // Utilisation de sendIrcMessage pour ERR_NEEDMOREPARAMS
        sendIt(ERR_NEEDMOREPARAMS(Client_tmp[iterator]->getNickname(), command), fd);
        return;
    }

    if (Client_tmp[iterator]->getRegistered()) {
        // Utilisation de sendIrcMessage pour ERR_ALREADYREGISTRED
        sendIt(ERR_ALREADYREGISTRED(Client_tmp[iterator]->getNickname()), fd);
        return;
    }

    if (!Client_tmp[iterator]->getRegistered() &&
        (Client_tmp[iterator]->getNickCheck() || Client_tmp[iterator]->getUserCheck())) {
        sendIt(ERR_BADCOMMANDORDER(Client_tmp[iterator]->getNickname()), fd);
        exec("QUIT");
        return;
    }
    Client_tmp[iterator]->setPassCheck(true);
    Client_tmp[iterator]->setClientPassword(password);
}

void command::nick(const std::string &client_data) {

    std::vector<client*>& Client_tmp = _server.getClientList();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;
    size_t iterator = _server.getIterator() - 1;

    std::istringstream iss(client_data);
    std::string command;
    std::string nickname;
    iss >> command;
    iss >> nickname;
    P << nickname << E;
    if (nickname.empty()) {
        sendIt(ERR_NEEDMOREPARAMS(Client_tmp[iterator]->getNickname(), command), fd);
        exec("QUIT");
        return;
    }
    if(Client_tmp[iterator]->getRegistered())
    {
        //CHECK SI CE NICK EST DEJA REGISTERED SUR LE SERV, SI OUI , REFUSER LE NOUVEAU NICK, DONC LAISSER L'ACTUEl
        Client_tmp[iterator]->setNickname(nickname);
        std::string message = "You are now known as " + nickname + "\n";
        send(fd, message.c_str(), message.size(), 0);
        return ;
    }
    if(Client_tmp[iterator]->getPassCheck() && Client_tmp[iterator]->getUserCheck() == false && Client_tmp[iterator]->getNickCheck() == false)
    {
        //CHECK SI CE NICK EST DEJA REGISTERED SUR LE SERV, SI OUI , REFUSER LA CONNECTION
        Client_tmp[iterator]->setNickCheck(true);
        Client_tmp[iterator]->setNickname(nickname);
        return ;
    }
    if(Client_tmp[iterator]->getPassCheck() == false || Client_tmp[iterator]->getNickCheck())
    {
        sendIt(ERR_BADCOMMANDORDER(Client_tmp[iterator]->getNickname()), fd);
        exec("QUIT");
        return ;
    }
}

void command::user(const std::string &client_data) 
{
    std::vector<client*>& Client_tmp = _server.getClientList();
    size_t iterator = _server.getIterator() - 1;
    std::istringstream iss(client_data);
    std::string command;
    std::string username;
    std::string nothing = "Unknown";
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();


    int fd = pollfd_tmp[_server.getIterator()].fd;
    iss >> command;
    iss >> username;
    if (username.empty())
    {
        sendIt(ERR_NEEDMOREPARAMS(nothing, command), fd);
        exec("QUIT");
        return ;
    }
    if(Client_tmp[iterator]->getRegistered())
    {
        std::string nickname = Client_tmp[iterator]->getNickname();
        sendIt(ERR_ALREADYREGISTRED(nickname), fd);
        return ;
    }
    if (Client_tmp[iterator]->getPassCheck() == false || Client_tmp[iterator]->getNickCheck() == false)
    {
        std::string message = "Commands bad order during connection, PASS/NICK/USER\nClient Disconnected\n";
        send(fd, message.c_str(), message.size(), 0);
        exec("QUIT");
        return ;
    }
    if (Client_tmp[iterator]->getPassCheck() && Client_tmp[iterator]->getNickCheck())
    {
        Client_tmp[iterator]->setUserCheck(true);
        Client_tmp[iterator]->setUsername(username);
        if (Client_tmp[iterator]->getClientPassword() == _server.getPassword())
        {
            std::string nickname = Client_tmp[iterator]->getNickname();
            sendIt(RPL_WELCOME(nickname), fd);
            Client_tmp[iterator]->setRegistered(true);
        }   
        else
        {
            std::string message = "Wrong Password\nClient Disconnected\n";
            send(fd, message.c_str(), message.size(), 0);
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
    size_t iterator = _server.getIterator() - 1;
    std::string nickname = Client_tmp[iterator]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    bool channelExists = false;
    for (size_t x = 0; x < Channel_tmp.size(); x++) {
        if (Channel_tmp[x]->getName() == channel_name) {
            channelExists = true;
            if (!Channel_tmp[x]->IsInChannel(nickname)) {
                Channel_tmp[x]->addClient(Client_tmp[iterator]);
                P << "Client added to existing channel" << E;
            }
            break;
        }
    }

    if (!channelExists) {
        channel* new_channel = new channel(channel_name);
        new_channel->addClient(Client_tmp[iterator]);
        new_channel->addOperator(Client_tmp[iterator]);
        _server.addChannel(new_channel);
        P << "Channel created and client added" << B_R << "(Operator) " << RESET << new_channel->getName() << E;
    }

    (void)fd;
    // Send RPL TOPIC
    std::string topic = "Welcome to the channel!";
    sendIt(RPL_TOPIC(nickname, channel_name, topic), fd);

    // Send JOIN message
    std::string join_message = ":" + nickname + " JOIN " + channel_name + "\r\n";
    send(pollfd_tmp[_server.getIterator()].fd, join_message.c_str(), join_message.size(), 0);



    // Send JOIN message
    // _server.sendIrcMessage(_server.getServerName(), "JOIN", nickname, channel_name, "", fd);

    // Send RPL_TOPIC (332) message
    // std::string topic = "Welcome to the channel!";
    // _server.sendIrcMessage(_server.getServerName(), "RPL_TOPIC", nickname, channel_name, topic, fd);

    // Send RPL_NAMREPLY (353) message
   // std::string names_list = nickname; // Add other users in the channel if needed
   // _server.sendIrcMessage(_server.getServerName(), "RPL_NAMREPLY", nickname, channel_name, names_list, fd);

    // Send RPL_ENDOFNAMES (366) message
    //_server.sendIrcMessage(_server.getServerName(), "RPL_ENDOFNAMES", nickname, channel_name, "", fd);

    _server.printChannelsAndClients();
}

bool ft_is_mode(char c)
{
    if (c == '+' || c == '-' || c == 'i' || c == 't' || c == 'k' || c == 'o' || c == 'l')
        return true;
    return false;
}

std::string checker_flag(const std::string &flag)
{
    std::string Error;
    size_t x = 0;
    while (x < flag.size() && ft_is_mode(flag[x]))
        x++;
    if (x == flag.size())
        return ("");
    Error = flag[x];
    return Error;

}

std::string command::get_previous_sign(std::string flag)
{
    size_t count = iterator_mode;
    while(count > 0 && (flag[count] != '+' && flag[count] != '-'))
        count--;
    if (count == 0 && (flag[count] != '+' && flag[count] != '-'))
        return "+";
    std::string sign;
    sign = flag[count];
    return (sign);
}

std::pair<std::string,std::string> command::get_mode_and_sign(std::string flag)
{
    while (iterator_mode < flag.size() && (flag[iterator_mode] == '+' || flag[iterator_mode] == '-'))
        iterator_mode++;
    std::string mode;
    mode = flag[iterator_mode];
    std::string sign;
    sign = get_previous_sign(flag);
    return (std::make_pair(sign, mode));
}

std::vector<std::pair<std::string,std::string> > command::parsing_param_mode(const std::string &client_data)
{
    std::istringstream iss(client_data);
    std::string command;
    std::string channel_name;
    std::string flag;

    iss >> command;
    iss >> channel_name;
    iss >> flag;

    std::vector<client*>& Client_tmp = _server.getClientList();
    size_t iterator = _server.getIterator() - 1;
    std::string nickname = Client_tmp[iterator]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    std::string Error;
    Error = checker_flag(flag);
    std::vector<std::pair<std::string,std::string> > arguments;
    if (Error.size() > 0)
    {
        sendIt(ERR_UNKNOWNMODE(nickname, Error), fd);
        return arguments;
    }
    while (iterator_mode < flag.size())
    {
        std::pair<std::string,std::string> tmp = get_mode_and_sign(flag);
        arguments.push_back(tmp);
        iterator_mode++;
    }
    for (size_t x = 0; x < arguments.size(); x++)
    {
        if (!ft_is_mode(arguments[x].second[0]))
        {
            arguments.erase(arguments.begin() + x);
        }
    }
    return arguments;
    //Checker de bonne mise en forme, que + et -, puis que des arguments valides, sinnon error;
    //std::vector<std::pair<std::string,std::string> > Arg;
}

void command::mode(const std::string &client_data) { //MODE #cc +i
    std::istringstream iss(client_data);
    std::string command;
    std::string channel_name;
    std::string flag;

    iss >> command;
    iss >> channel_name;
    iss >> flag;

    // std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    std::vector<client*>& Client_tmp = _server.getClientList();
    size_t iterator = _server.getIterator() - 1;
    std::string nickname = Client_tmp[iterator]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    if (channel_name.empty() && flag.empty())
    {
        sendIt(ERR_NEEDMOREPARAMS(nickname, command), fd);
        return ;
    }
    if (channel_name[0] != '#')
    {
        sendIt(ERR_NOSUCHNICK(nickname, channel_name), fd);
        //:Armida.german-elite.net 401 pierre -t :No such nick/channel
        return ;
    }
    if(flag.empty())
        sendIt(RPL_CHANNELMODEIS(nickname, channel_name, flag), fd); //affiche modes
    else
    {
        //APPELLE FONCTION PIERRERULENCE, DECOUPE client_data et envoie a pierrot l'asticot le reste.
        std::vector<std::pair<std::string,std::string> > arguments = parsing_param_mode(client_data);
        for (size_t x = 0; x < arguments.size(); x++)
        {
            P << B_G << "Arguments[" << x << "] => " << arguments[x].first << RESET << E;
            P << B_G << "Arguments[" << x << "] => " << arguments[x].second << RESET << E << E;
        }
        std::vector<channel*>& Channel_tmp = _server.getChannelsList();
        for (size_t x = 0; x < Channel_tmp.size(); x++)
        {
            if (Channel_tmp[x]->getName() == channel_name) 
                Channel_tmp[x]->setChannelFlag(flag);
        }
    }
    //:Armida.german-elite.net 324 pierre #io +nt
    //@time=2025-03-21T12:15:57.951Z :pierre!~pierrre@GE-9042C255.unyc.it MODE #rr +i // changement de mode
}

void command::quit(const std::string &client_data) {
    (void)client_data;
    std::vector<client*>& Client_tmp = _server.getClientList();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    size_t iterator = _server.getIterator();

    if (iterator < 1 || iterator > Client_tmp.size()) {
        P << " invalid iterator: " << iterator << E;
        return;
    }

    // Supprimer le client de manière sécurisée
    client* client_to_remove = Client_tmp[iterator - 1];
    if (client_to_remove) {
        P << "Removing client: " << client_to_remove->getNickname() << E;
        delete client_to_remove;
        Client_tmp.erase(Client_tmp.begin() + (iterator - 1));
        close(pollfd_tmp[iterator].fd); // Mettre le pointeur à NULL
        pollfd_tmp.erase(pollfd_tmp.begin() + (iterator));
    } else {
        P << "Client pointer is null, skipping removal" << E;
    }
}


size_t command::getChanIterator(std::string channelname)
{
    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    for (size_t x = 0; x < Channel_tmp.size(); x++)
    {
        if (Channel_tmp[x]->getName() == channelname) 
            return x;
    }
    return static_cast<size_t>(-1);
}

void command::topic(const std::string &client_data) {
    std::istringstream iss(client_data);
    std::string command;
    std::string channel_name;
    std::string topic_name;

    iss >> command;
    iss >> channel_name;
    iss >> topic_name;
        

    if (!topic_name.empty() && topic_name[0] == ' ') {
        topic_name.erase(0, 1);
    }

    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    std::vector<client*>& Client_tmp = _server.getClientList();
    std::string nickname = Client_tmp[_server.getIterator() - 1]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;
    size_t x = getChanIterator(channel_name);

    if (topic_name.empty())
    {
        if(channel_name.size() > 1  && channel_name[0] == '#')
        {
            for (size_t x = 0; x < Channel_tmp.size(); x++)
            {
                if (Channel_tmp[x]->getName() == channel_name) 
                {
                    if (Channel_tmp[x]->getTopic().empty()){
                        sendIt(RPL_NOTOPIC(nickname, channel_name), fd);
                        return ;
                    }
                    else{
                        sendIt(RPL_TOPIC(nickname, channel_name, Channel_tmp[x]->getTopic()), fd);
                        return;
                    }
                }
            }
            std::string chan = "NONE";
            sendIt(ERR_NOSUCHCHANNEL(nickname, channel_name), fd);
            return;
        }
        else
        {
            for (size_t x = 0; x < Channel_tmp.size(); x++)
            {
                if (Channel_tmp[x]->getName() == channel_name) //verifier que /topic est execute dans un channel existant et dont je fais partit et dont je suis OP
                {
                    if (Channel_tmp[x]->IsInChannel(nickname))
                    {
                        //AJOUTER LA CONDITION SI CLIENT EST DANS LE CHAN ET OP, QUE LES DROITS TOPIC SONT OPERATOR OU NON
                        Channel_tmp[x]->setTopic(channel_name);
                        //_server.sendIrcMessage(_server.getServerName(), "TOPIC", nickname, Channel_tmp[x]->getName(), channel_name, fd);
                        P << B_Y << "Channel topic changed to: " << B_R << Channel_tmp[x]->getTopic() << RESET << E; return;
                    }
                    else
                    {
                        //:Armida.german-elite.net 442 HELL #tata :You're not on that channel // ERR_NOTONCHANNEL
                        sendIt(ERR_NOTONCHANNEL(nickname, channel_name), fd);
                    }
                }
            }
            sendIt(ERR_NOSUCHCHANNEL(nickname, channel_name), fd);
        }
    }
    if(channel_name.size() > 0 && channel_name[0] == '#' && topic_name.size() > 0)
    {
        Channel_tmp[x]->setTopic(topic_name);
    }

    for (size_t x = 0; x < Channel_tmp.size(); x++) {
        if (Channel_tmp[x]->getName() == channel_name) {
            if (topic_name.empty()) {
                if (Channel_tmp[x]->getTopic().empty()) {
                    sendIt(RPL_NOTOPIC(nickname, channel_name), fd);
                } else {
                    sendIt(RPL_TOPIC(nickname, channel_name, Channel_tmp[x]->getTopic()), fd);
                }
            } else {
                Channel_tmp[x]->setTopic(topic_name);
                sendIt(RPL_TOPIC(nickname, channel_name, Channel_tmp[x]->getTopic()), fd);
                P << B_Y << "Channel topic changed to: " << B_R << Channel_tmp[x]->getTopic() << RESET << E;
            }
            return;
        }
    }

    sendIt(ERR_NOSUCHCHANNEL(nickname, channel_name), fd);
}

void    command::cap(const std::string &client_data)
{
    (void)client_data;
    return ;   
}

command::command(Server& server) : _server(server)
{
    iterator_mode = 0;
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
        size_t iterator = _server.getIterator() - 1;
        if (Client_tmp[iterator]->getRegistered() == false)
        {
            std::string message = "Command not found during connection PASS/NICK/USER\nClient Disconnected\n";
            std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
            send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
            exec("QUIT");
            return ;
        }
        std::string message = "Command not found\n";
        std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
        send(pollfd_tmp[_server.getIterator()].fd, message.c_str(), message.size(), 0);
    }
}

void command::sendIt(std::string def, int fdClient)
{
    def += "\r\n";
    def.insert(0, ":" + _server.getServerName() + " ");

    send(fdClient, def.c_str(), def.size(), 0);

}