/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 15:41:23 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 15:41:25 by lvan-slu         ###   ########.fr       */
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

void    command::invite(const std::string &client_data)
{
	std::istringstream iss(client_data);
	std::string command;
	std::string targetNickname;
	std::string channelName;
	
	iss >> command >> targetNickname >> channelName;

	std::vector<client*>& Client_tmp = _server.getClientList(); // recupere la liste des clients
	std::vector<channel*>& Channel_tmp = _server.getChannelsList(); // recupere la liste des channels
	std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd(); // recupere la liste des pollfd

	size_t iterator = _server.getIterator() - 1;
	std::string senderNickname = Client_tmp[iterator]->getNickname();
	int sender_fd = pollfd_tmp[_server.getIterator()].fd;
	(void)sender_fd;

	//verif du # avant channel + un des 2 arguments n est pas vide
	if (channelName[0] != '#' || targetNickname.empty() || channelName.empty())
	{
		sendIt(ERR_NEEDMOREPARAMS(senderNickname, "INVITE"), sender_fd);
		return ;
	}
	// 0. Verifie si le channel existe
	for (size_t i = 0; i < static_cast<size_t>(Channel_tmp.size()); i++)
	{
		if (Channel_tmp[i]->getName() == channelName) // trouver le channel
			break;
		if (i == Channel_tmp.size() - 1)
		{
			sendIt(ERR_NOSUCHCHANNEL(senderNickname, channelName), sender_fd);
			return ;
		}
	}
	// 1. Verifie si le client est bien dans le canal dans lequel il veut inviter
	for (size_t i = 0; i < static_cast<size_t>(Channel_tmp.size()); i++)
	{
		if (Channel_tmp[i]->getName() == channelName) // trouver le channel
		{
			if (!Channel_tmp[i]->IsInChannel(senderNickname)) // trouve celui qui invite
			{
				sendIt(ERR_NOTONCHANNEL(senderNickname, channelName), sender_fd);
				return ;
			}
			if (Channel_tmp[i]->IsInChannel(targetNickname)) // trouver la target a inviter
			{
				sendIt(ERR_USERONCHANNEL(senderNickname, targetNickname, channelName), sender_fd);
				return ;
			}
			else
			{
				// Trouver le client à inviter
				int target_fd = -1;
				bool targetExists = false;
				
				for (size_t j = 0; j < Client_tmp.size(); j++) {
					if (Client_tmp[j]->getNickname() == targetNickname) {
						targetExists = true;
						target_fd = pollfd_tmp[j + 1].fd;
						break;
					}
				}
				
				if (targetExists) {
					// Envoyer l'invitation
					std::string inviteMessage = ":" + senderNickname + " INVITE " + targetNickname + " " + channelName + "\r\n";
					send(target_fd, inviteMessage.c_str(), inviteMessage.size(), 0);
					
					// Confirmer à l'expéditeur
					sendIt(RPL_INVITING(senderNickname, targetNickname, channelName), sender_fd);
				} else {
					sendIt(ERR_NOSUCHNICK(senderNickname, targetNickname), sender_fd);
				}
				break;
			}
		}
	}
}

