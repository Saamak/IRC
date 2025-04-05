
#include "includes/command.hpp"

void command::invite(const std::string &client_data)
{
	std::istringstream iss(client_data);
	std::string command, targetNickname, channelName;
	
	iss >> command >> targetNickname >> channelName;
	std::string senderNickname = getSenderNickname();
	int sender_fd = getSenderFd();

	try 
	{
		if (channelName[0] != '#' || targetNickname.empty() || channelName.empty())
			throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, "INVITE"));
		
		channel* targetChannel = getChannel(channelName);
		if (!targetChannel)
			throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channelName));
			
		if (!targetChannel->IsInChannel(senderNickname))
			throw IrcException("ERR_NOTONCHANNEL", ERR_NOTONCHANNEL(senderNickname, channelName));
				
		if (!_server.clientExists(targetNickname))
			throw IrcException("ERR_NOSUCHNICK", ERR_NOSUCHNICK(senderNickname, targetNickname));
		
		if (targetChannel->IsInChannel(targetNickname))
			throw IrcException("ERR_USERONCHANNEL", ERR_USERONCHANNEL(senderNickname, targetNickname, channelName));

		int target_fd = _server.getClientFd(targetNickname);
		targetChannel->addInvite(targetNickname);
		sendIt(RPL_INVITING(senderNickname, targetNickname, channelName), sender_fd);
		std::cout << "[DEBUG] " << targetNickname << " has been invited to " << channelName << std::endl;
		std::string inviteMessage = ":" + senderNickname + " INVITE " + targetNickname + " " + channelName + "\r\n";
		send(target_fd, inviteMessage.c_str(), inviteMessage.size(), 0);
	}
	catch (const IrcException& e) 
	{
		sendIt(e.getErrorMsg(), sender_fd);
	}
}
