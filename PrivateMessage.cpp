/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivateMessage.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 13:04:18 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 13:04:21 by lvan-slu         ###   ########.fr       */
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

// Commande PRIVMSG : Envoie un message à un utilisateur ou un canal : /PRIVMSG <destinataire> :<message>
// Exemple : /PRIVMSG #channel :Hello, world!

void	command::processPrivateMessage(const std::string& targetUser, const std::string& message, const std::string& nickname, int sender_fd)
{
	std::vector<client*>& Client_tmp = _server.getClientList();
	std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
	
	for (size_t i = 0; i < Client_tmp.size(); i++) 
	{
		if (Client_tmp[i]->getNickname() == targetUser)
		{
			std::string formattedMessage = ":" + nickname + " PRIVMSG " + targetUser + " :" + message + "\r\n";
			send(pollfd_tmp[i + 1].fd, formattedMessage.c_str(), formattedMessage.size(), 0);
			return;
		}
	}
	sendIt(ERR_NOSUCHNICK(nickname, targetUser), sender_fd);
}

void command::processChannelMessage(const std::string& targetChannel, const std::string& message, const std::string& nickname, int sender_fd)
{
	std::vector<channel*>& Channel_tmp = _server.getChannelsList();
	std::vector<client*>& Client_tmp = _server.getClientList();

	for (size_t i = 0; i < Channel_tmp.size(); i++) 
	{
		if (Channel_tmp[i]->getName() == targetChannel) 
		{
			if (!Channel_tmp[i]->IsInChannel(nickname)) 
			{
				sendIt(ERR_CANNOTSENDTOCHAN(nickname, targetChannel), sender_fd);
				return;
			}
			std::string formattedMessage = ":" + nickname + " PRIVMSG " + targetChannel + " :" + message + "\r\n";
			std::vector<client*> clients = Channel_tmp[i]->getClients();
			std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
		
			// Remplacer la boucle imbriquée existante par:
			for (size_t j = 0; j < clients.size(); j++) 
			{
				// Ignorer l'expéditeur
				if (clients[j]->getNickname() == nickname)
					continue;
				// Trouver le descripteur de fichier correspondant au client
				int target_fd = -1;
				for (size_t k = 1; k < pollfd_tmp.size(); k++) 
				{
					// On cherche le bon client en comparant avec sa position dans Client_tmp
					for (size_t l = 0; l < Client_tmp.size(); l++) {
						if (Client_tmp[l] == clients[j] && pollfd_tmp[k].fd > 0) {
							target_fd = pollfd_tmp[l + 1].fd; // +1 pour le décalage serveur
							break;
						}
					}
					if (target_fd != -1)
						break;
				}
			// Envoyer le message si on a trouvé le fd
				if (target_fd != -1)
			   		send(target_fd, formattedMessage.c_str(), formattedMessage.size(), 0);
			}
			return;
		}
	}
	sendIt(ERR_NOSUCHCHANNEL(nickname, targetChannel), sender_fd);
}

void command::privmsg(const std::string &client_data) 
{
	std::string targetType;
	std::string message;

	size_t pos = client_data.find(' ');
	if (pos != std::string::npos) 
	{
		size_t targetPos = client_data.find(' ', pos + 1);
		if (targetPos != std::string::npos) 
		{
			targetType = client_data.substr(pos + 1, targetPos - pos - 1);
			size_t messagePos = client_data.find(':', targetPos);
			if (messagePos != std::string::npos) 
				message = client_data.substr(messagePos + 1);
		}
	}
	
	std::vector<client*>& Client_tmp = _server.getClientList();
	size_t iterator = _server.getIterator() - 1;
	std::string nickname = Client_tmp[iterator]->getNickname();
	std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
	int sender_fd = pollfd_tmp[_server.getIterator()].fd;
	
	if (targetType.empty() || message.empty()) 
	{
		if (targetType.empty())
			sendIt(ERR_NORECIPIENT(nickname), sender_fd);
		else
			sendIt(ERR_NOTEXTTOSEND(nickname), sender_fd);
		return;
	}

	if (targetType[0] == '#')
		processChannelMessage(targetType, message, nickname, sender_fd);
	else 
		processPrivateMessage(targetType, message, nickname, sender_fd);
}
