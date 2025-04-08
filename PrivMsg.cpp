
#include "includes/command.hpp"

void	command::processPrivateMessage(const std::string& targetUser, const std::string& message, const std::string& nickname)
{
	if (!_server.clientExists(targetUser))
		throw IrcException("ERR_NOSUCHNICK", ERR_NOSUCHNICK(nickname, targetUser));
		
	int target_fd = _server.getClientFd(targetUser);
	std::string formattedMessage = ":" + nickname + " PRIVMSG " + targetUser + " :" + message + "\r\n";
	send(target_fd, formattedMessage.c_str(), formattedMessage.size(), 0);
}

void command::processChannelMessage(const std::string& targetChannel, const std::string& message, const std::string& nickname)
{
	channel* targetChan = getChannel(targetChannel);
	if (!targetChan)
		throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(nickname, targetChannel));
		
	if (!targetChan->IsInChannel(nickname))
		throw IrcException("ERR_CANNOTSENDTOCHAN", ERR_CANNOTSENDTOCHAN(nickname, targetChannel));
		
	std::string formattedMessage = ":" + nickname + " PRIVMSG " + targetChannel + " :" + message + "\r\n";
	
	std::vector<client*> channelClients = targetChan->getClients();
	
	for (size_t i = 0; i < channelClients.size(); i++)
	{
		if (channelClients[i]->getNickname() == nickname)
			continue;
			
		int client_fd = _server.getClientFd(channelClients[i]->getNickname());
		
		if (client_fd != -1)
			send(client_fd, formattedMessage.c_str(), formattedMessage.size(), 0);
	}
}

void command::privmsg(const std::string &client_data) 
{
	std::string targetType, message;
	std::string senderNickname = getSenderNickname();
	int sender_fd = getSenderFd();

	try 
	{
		size_t pos = client_data.find(' ');
		if (pos == std::string::npos) 
			throw IrcException("ERR_NORECIPIENT", ERR_NORECIPIENT(senderNickname));
			
		size_t targetPos = client_data.find(' ', pos + 1);
		if (targetPos == std::string::npos)
			throw IrcException("ERR_NORECIPIENT", ERR_NORECIPIENT(senderNickname));
			
		targetType = client_data.substr(pos + 1, targetPos - pos - 1);
		if (targetType.empty())
			throw IrcException("ERR_NORECIPIENT", ERR_NORECIPIENT(senderNickname));
			
		size_t messagePos = client_data.find(':', targetPos);
		if (messagePos == std::string::npos)
			throw IrcException("ERR_NOTEXTTOSEND", ERR_NOTEXTTOSEND(senderNickname));
			
		message = client_data.substr(messagePos + 1);
		if (message.empty())
			throw IrcException("ERR_NOTEXTTOSEND", ERR_NOTEXTTOSEND(senderNickname));

		if (targetType[0] == '#')
			processChannelMessage(targetType, message, senderNickname);
		else 
			processPrivateMessage(targetType, message, senderNickname);
	}
	catch (const IrcException& e)
	{
		sendIt(e.getErrorMsg(), sender_fd);
	}
}