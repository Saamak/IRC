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
	void	createChannel(const std::string& channel_name, const std::string& password, const std::string& senderNickname, int sender_fd);
	void    cap(const std::string &client_data);
	size_t getChanIterator(std::string channelname);
	// void part();
	void    privmsg(const std::string &client_data);
	void    invite(const std::string &client_data);
	void    processChannelMessage(const std::string& targetChannel, const std::string& message, const std::string& nickname);
	void    processPrivateMessage(const std::string& targetChannel, const std::string& message, const std::string& nickname);
	// void notice();
	void    quit(const std::string &client_data);
	void    topic(const std::string &client_data);
	void    mode(const std::string &client_data);
	void    plusSignMode(std::string channel_name, std::string mode, std::string senderNickname, int sender_fd, std::string password);
	void    minusSignMode(std::string channel_name, std::string mode, std::string senderNickname, int sender_fd);
	void    kick(const std::string &client_data);
	// void invite();
	// void who();
	// void whois();
	// void whowas();

	void sendIt(std::string def, int fdClient);
	std::vector<std::pair<std::string,std::string> > parsing_param_mode(const std::string &client_data);
	std::pair<std::string,std::string> get_mode_and_sign(std::string flag);
	std::string get_previous_sign(std::string flag);

	std::string getSenderNickname();
	int getSenderFd();
	client* getSender();
	channel* getChannel(const std::string &channelName);
};

#endif // COMMAND_HPP