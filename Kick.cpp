/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 17:06:21 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 17:06:22 by lvan-slu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/colors.h"
#include "includes/command.hpp"
#include "includes/channel.hpp"
#include "IrcException.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib> 
#include <utility>

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
					break;
				}
			}
			return;
		}
	}
}

void command::kick(const std::string &client_data)
{
	std::istringstream iss(client_data);
	std::string command, targetNickname, channelName;
	
	iss >> command >> channelName >> targetNickname;
	std::string senderNickname = getSenderNickname();
	int sender_fd = getSenderFd();

	try 
	{
		if (channelName[0] != '#' || targetNickname.empty() || channelName.empty())
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
		
		int target_fd = _server.getClientFd(targetNickname);
		std::string kickMessage = ":" + senderNickname + " KICK " + targetNickname + " " + channelName+ " :You have been kicked\r\n";
		send(target_fd, kickMessage.c_str(), kickMessage.size(), 0);
		
		targetChannel->removeUser(targetNickname);
		
		std::string channelNotice = ":" + senderNickname + " KICK " + channelName + " " + targetNickname + " :Kicked by " + senderNickname + "\r\n";
		std::vector<client*> channelClients = targetChannel->getClients();
		for (size_t i = 0; i < channelClients.size(); i++)
		{
			int client_fd = _server.getClientFd(channelClients[i]->getNickname());
			send(client_fd, channelNotice.c_str(), channelNotice.size(), 0);
		}
	}
	catch (const IrcException& e) {
		sendIt(e.getErrorMsg(), sender_fd);
	}
}
