#include "includes/command.hpp"
#include <iostream>
#include "includes/colors.h"
#include <sstream>
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"
#include <cstdlib> 
#include <utility>

client* command::getSender() 
{
	std::vector<client*>& clients = _server.getClientList();
	size_t iterator = _server.getIterator() - 1;
	return clients[iterator];
}

channel* command::getChannel(const std::string &channelName) 
{
	std::vector<channel*>& channels = _server.getChannelsList();
	for (size_t i = 0; i < channels.size(); i++) 
	{
		if (channels[i]->getName() == channelName)
		return channels[i];
	}
	return NULL;
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
	_cmds["PRIVMSG"] = &command::privmsg;
	// _cmds["NOTICE"] = &command::notice;
	_cmds["QUIT"] = &command::quit;
	_cmds["TOPIC"] = &command::topic;
	_cmds["MODE"] = &command::mode;
	_cmds["KICK"] = &command::kick;
	_cmds["INVITE"] = &command::invite;
	_cmds["WHO"] = &command::who;
	// _cmds["WHOIS"] = &command::whois;
	// _cmds["WHOWAS"] = &command::whowas;
}

void command::exec(const std::string &client_data) 
{
	// _cmds est un map qui associe une commande à une fonction
	std::stringstream iss(client_data);
	std::string buff;
	iss >> buff; //recupere le 1er mot de la cmd (JOIN etc)
	std::map<std::string, CommandFunction>::iterator it = _cmds.find(buff); // recherche si la command existe
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

std::string command::getSenderNickname() 
{
	std::vector<client*>& clients = _server.getClientList();
	size_t iterator = _server.getIterator() - 1;
	return clients[iterator]->getNickname();
}

int command::getSenderFd() {return (_server.getPollFd()[_server.getIterator()].fd);}

command::~command(){ P <<BOLD<< "COMMAND DESTRUCTEUR" <<RESET<< E;}