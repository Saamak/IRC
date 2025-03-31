/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:30:09 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 11:30:10 by lvan-slu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/command.hpp"
#include "includes/IrcException.hpp"
#include <iostream>
#include "includes/colors.h"
#include <sstream>
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"
#include <cstdlib> 
#include <utility>

void command::createChannel(const std::string& channel_name, const std::string& password, const std::string& senderNickname, int sender_fd)
{
	channel* newChannel = new channel(channel_name);
	newChannel->addClient(getSender());
	newChannel->addOperator(getSender());

	if (!password.empty())
	{
		newChannel->setIsPasswd(true);
		newChannel->setKey(password);
	}

	_server.addChannel(newChannel);

	std::string joinMessage = ":" + senderNickname + " JOIN " + channel_name + "\r\n";
	send(sender_fd, joinMessage.c_str(), joinMessage.size(), 0);

	std::string defaultTopic = "Welcome to the channel!";
	newChannel->setTopic(defaultTopic);
	sendIt(RPL_TOPIC(senderNickname, channel_name, defaultTopic), sender_fd);

	sendIt(RPL_NAMREPLY(senderNickname, channel_name, senderNickname), sender_fd);
	sendIt(RPL_ENDOFNAMES(senderNickname, channel_name), sender_fd);
}

void command::join(const std::string &client_data)
{
	std::istringstream iss(client_data);
	std::string command, channel_name, password;

	iss >> command >> channel_name >> password;

	std::string senderNickname = getSenderNickname();
	int sender_fd = getSenderFd();

	try
	{
		if (channel_name.empty())
			throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, "JOIN"));
		if (channel_name[0] != '#')
			throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channel_name));
		
		channel* targetChannel = getChannel(channel_name);
		std::cout << channel_name << senderNickname << std::endl;
		if (targetChannel)
		{
			if (targetChannel->getIsInvitOnly() && !targetChannel->isInvited(senderNickname))
				throw IrcException("ERR_INVITEONLYCHAN", ERR_INVITEONLYCHAN(senderNickname, channel_name));
			if (targetChannel->getIsPasswd() && targetChannel->getKey() != password)
				throw IrcException("ERR_BADCHANNELKEY", ERR_BADCHANNELKEY(senderNickname, channel_name));
			if (targetChannel->IsInChannel(senderNickname))
				throw IrcException("ERR_USERONCHANNEL", ERR_USERONCHANNEL(senderNickname, channel_name, senderNickname));
			
			targetChannel->addClient(getSender());
			std::string joinMessage = ":" + senderNickname + " JOIN " + channel_name + "\r\n";
			send(sender_fd, joinMessage.c_str(), joinMessage.size(), 0);
			if (!targetChannel->getTopic().empty())
				sendIt(RPL_TOPIC(senderNickname, channel_name, targetChannel->getTopic()), sender_fd);
			else
				sendIt(RPL_NOTOPIC(senderNickname, channel_name), sender_fd);
			const std::vector<client*>& clients = targetChannel->getClients();
			std::string usersList;
			for (std::vector<client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
			{
				if (!usersList.empty())
					usersList += " ";
				usersList += (*it)->getNickname();
			}

			sendIt(RPL_NAMREPLY(senderNickname, channel_name, usersList), sender_fd);
			sendIt(RPL_ENDOFNAMES(senderNickname, channel_name), sender_fd);

			
			return;
		}

		createChannel(channel_name, password, senderNickname, sender_fd);
	}
	catch (const IrcException& e)
	{
		sendIt(e.getErrorMsg(), sender_fd);
	}
}