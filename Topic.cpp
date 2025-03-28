/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 12:39:43 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/28 12:39:44 by lvan-slu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/command.hpp"
#include <iostream>
#include "includes/colors.h"
#include <sstream>
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"
#include <cstdlib> 
#include <utility>

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
