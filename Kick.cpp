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

#include "includes/command.hpp"
#include <iostream>
#include "includes/colors.h"
#include <sstream>
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"
#include <cstdlib> 
#include <utility>

void channel::removeUser(const std::string &nickname)
{
    // Find the client with the given nickname
    for (std::vector<client*>::iterator it = client_lst.begin(); it != client_lst.end(); ++it) {
        if ((*it)->getNickname() == nickname) {
            client_lst.erase(it);
            
            // Also check if they were an operator and remove them from that list too
            for (std::vector<client*>::iterator op_it = operator_lst.begin(); op_it != operator_lst.end(); ++op_it) {
                if ((*op_it)->getNickname() == nickname) {
                    operator_lst.erase(op_it);
                    break;
                }
            }
            return;
        }
    }
}

void command::removeUserCommand(const std::string &nickname, const std::string &channelName)
{
	std::cout << "[DEBUG] User " << std::endl;
    // Récupère la liste des canaux
    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    
    // Cherche le canal correspondant
    for (size_t i = 0; i < Channel_tmp.size(); i++)
    {
        if (Channel_tmp[i]->getName() == channelName)
        {
            // Vérifie si l'utilisateur est dans le canal
            if (Channel_tmp[i]->IsInChannel(nickname))
            {
                // Supprime l'utilisateur du canal
                Channel_tmp[i]->removeUser(nickname);
                std::cout << "Utilisateur " << nickname << " a été expulsé du canal " << channelName << std::endl;
            }
            else
            {
                std::cout << "Utilisateur " << nickname << " n'est pas dans le canal " << channelName << std::endl;
            }
            break;
        }
    }
}

void    command::kick(const std::string &client_data)
{
	std::cout << "[DEBUG] Processing KICK command: " << client_data << std::endl;
	std::istringstream iss(client_data);
	std::string command;
	std::string targetNickname;
	std::string channelName;
	
	iss >> command >> channelName >> targetNickname;

	std::vector<client*>& Client_tmp = _server.getClientList(); // recupere la liste des clients
	std::vector<channel*>& Channel_tmp = _server.getChannelsList(); // recupere la liste des channels
	std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd(); // recupere la liste des pollfd

	size_t iterator = _server.getIterator() - 1;
	std::string senderNickname = Client_tmp[iterator]->getNickname(); // recupere le nickname du client qui kick
	int sender_fd = pollfd_tmp[_server.getIterator()].fd; // recupere le fd du client qui kick

	//verif du # avant channel + un des 2 arguments n est pas vide
	if (channelName[0] != '#' || targetNickname.empty() || channelName.empty())
	{
		sendIt(ERR_NEEDMOREPARAMS(senderNickname, "KICK"), sender_fd);
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
	// 1. Verifie si le client est bien dans le canal dans lequel il veut kick un client
	for (size_t i = 0; i < static_cast<size_t>(Channel_tmp.size()); i++)
	{
		if (Channel_tmp[i]->getName() == channelName) // trouver le channel
		{
			if (!Channel_tmp[i]->IsInChannel(senderNickname)) // trouve celui qui kick
			{
				sendIt(ERR_NOTONCHANNEL(senderNickname, channelName), sender_fd);
				return ;
			}
			int target_fd = -1;
			bool targetExists = false;
			
			for (size_t j = 0; j < Client_tmp.size(); j++)
			{
				if (Client_tmp[j]->getNickname() == targetNickname) 
				{
					targetExists = true;
					target_fd = pollfd_tmp[j + 1].fd; // recupere le fd du client a kick
					break;
				}
			}
			std::cout << "[DEBUG] LA "<< std::endl;
			if (targetExists) 
			{
				std::cout << "[DEBUG] User " << targetNickname << " has been kicked from " << channelName 
				<< " by " << senderNickname << std::endl;
				// Check if the sender is an operator in the channel
				// if (!Channel_tmp[i]->isOperator(senderNickname))
				// {
				// 	sendIt(ERR_CHANOPRIVSNEEDED(senderNickname, channelName), sender_fd);
				// 	return;
				// }
				
				if (!Channel_tmp[i]->IsInChannel(targetNickname))
				{
					sendIt(ERR_USERNOTINCHANNEL(senderNickname, targetNickname, channelName), sender_fd);
					return;
				}
				
				std::string kickMessage = ":" + senderNickname + " KICK " + channelName + " " + targetNickname + " :You have been kicked\r\n";
				send(target_fd, kickMessage.c_str(), kickMessage.size(), 0);
                removeUserCommand(targetNickname, channelName);
				std::string channelNotice = ":" + senderNickname + " KICK " + channelName + " " + targetNickname + " :Kicked by " + senderNickname + "\r\n";
			} 
			else 
				sendIt(ERR_NOSUCHNICK(senderNickname, targetNickname), sender_fd); // si le client a kick n'existe pas
			break;
		}
	}
}