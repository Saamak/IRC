#include "includes/command.hpp"

void channel::removeUser(const std::string &nickname)
{
	for (std::vector<client*>::iterator it = client_lst.begin(); it != client_lst.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
		{
			client_lst.erase(it);
			for (std::vector<client*>::iterator opIt = operator_lst.begin(); opIt != operator_lst.end(); ++opIt)
			{
				if ((*opIt)->getNickname() == nickname)
				{
					operator_lst.erase(opIt);
					break ;
				}
			}
			return ;
		}
	}
}

void command::kick(const std::string &client_data)
{
	std::string senderNickname = getSenderNickname();
	int sender_fd = getSenderFd();

	try 
	{
		std::istringstream iss(client_data);
		std::string command, channelName, targetNickname;
		
		iss >> command >> channelName >> targetNickname;
		
		std::string comment = "Kicked from channel";
		
		size_t colonPos = client_data.find(':', client_data.find(targetNickname));
		if (colonPos != std::string::npos) {
			comment = client_data.substr(colonPos + 1);
		}
		
		if (channelName[0] != '#' || channelName[0] != '&' || targetNickname.empty() || channelName.empty())
			throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, "KICK"));
		if (!_server.clientExists(targetNickname))
			throw IrcException("ERR_NOSUCHNICK", ERR_NOSUCHNICK(senderNickname, targetNickname));
		
		channel* targetChannel = getChannel(channelName);
		if (!targetChannel)
			throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channelName));
		if (!targetChannel->IsInChannel(senderNickname))
			throw IrcException("ERR_NOTONCHANNEL", ERR_NOTONCHANNEL(senderNickname, channelName));
		if (!targetChannel->IsInChannel(targetNickname))
			throw IrcException("ERR_USERNOTINCHANNEL", ERR_USERNOTINCHANNEL(senderNickname, targetNickname, channelName));
		if (!targetChannel->IsOperator(senderNickname))
			throw IrcException("ERR_CHANOPRIVSNEEDED", ERR_CHANOPRIVSNEEDED(senderNickname, channelName));
		
		targetChannel->removeUser(targetNickname);
		
		std::string kickMessage = ":" + senderNickname + " KICK " + channelName + " " + targetNickname + " :" + comment + "\r\n";
		
		int target_fd = _server.getClientFd(targetNickname);
		send(target_fd, kickMessage.c_str(), kickMessage.size(), 0);
		std::vector<client*> channelClients = targetChannel->getClients();
		if (targetChannel->getNumberClient() == 0)
		{
			_server.getChannelsList().erase(std::remove(_server.getChannelsList().begin(), _server.getChannelsList().end(), targetChannel), _server.getChannelsList().end());
			delete targetChannel;
		}
		for (size_t i = 0; i < channelClients.size(); i++)
		{
			int client_fd = _server.getClientFd(channelClients[i]->getNickname());
			send(client_fd, kickMessage.c_str(), kickMessage.size(), 0);
		}
	}
	catch (const IrcException& e) {
		sendIt(e.getErrorMsg(), sender_fd);
	}
}
