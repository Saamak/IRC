/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:16:23 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 11:16:24 by lvan-slu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/command.hpp"
#include <iostream>
#include "includes/colors.h"
#include <sstream>
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/client.hpp"
#include "includes/IrcException.hpp"
#include <cstdlib> 
#include <utility>

// MODE - Changer le mode du channel :
// — i : Définir/supprimer le canal sur invitation uniquement
// — t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
// — k : Définir/supprimer la clé du canal (mot de passe) ------------------------------------  [GOOD]
// — o : Donner/retirer le privilège de l’opérateur de canal
// — l : Définir/supprimer la limite d’utilisateurs pour le canal

// Mode i (Invitation uniquement)
// +i : Rend le canal accessible uniquement sur invitation, les utilisateurs ne peuvent rejoindre que s'ils sont explicitement invités
// -i : Supprime la restriction d'invitation, permettant à quiconque de rejoindre
// Mode t (Restrictions de sujet)
// +t : Seuls les opérateurs de canal peuvent changer le sujet
// -t : N'importe quel membre du canal peut changer le sujet
// Mode k (Clé/mot de passe du canal)
// +k [mot de passe] : Définit un mot de passe requis pour rejoindre le canal
// -k : Supprime l'exigence de mot de passe
// Mode o (Statut d'opérateur)
// +o [pseudo] : Accorde les privilèges d'opérateur à l'utilisateur spécifié
// -o [pseudo] : Retire les privilèges d'opérateur de l'utilisateur spécifié
// Mode l (Limite d'utilisateurs)
// +l [nombre] : Définit un nombre maximum d'utilisateurs autorisés dans le canal
// -l : Supprime la limite d'utilisateurs

//void	command::minusSignMode()

void	command::addModeK(std::string channel_name, std::string password)
{
	for (size_t x = 0; x < _server.getChannelsList().size(); x++)
	{
		if (_server.getChannelsList()[x]->getName() == channel_name)
		{
			_server.getChannelsList()[x]->setChannelFlag("+k");
			_server.getChannelsList()[x]->setTopic(password);
			return;
		}
	}
}

void command::minusSignMode(std::string channel_name, std::string mode, std::string senderNickname, int sender_fd)
{
	for (size_t x = 0; x < _server.getChannelsList().size(); x++)
	{
		if (_server.getChannelsList()[x]->getName() == channel_name)
		{
			if (mode == "k")
			{
				_server.getChannelsList()[x]->setChannelFlag("-k");
				sendIt("Password removed for channel " + channel_name, sender_fd);
				return;
			}
				else
					sendIt(ERR_UNKNOWNMODE(senderNickname, mode), sender_fd);

		}
	}
	sendIt(ERR_NOSUCHCHANNEL(senderNickname, channel_name), sender_fd);
}

void	command::plusSignMode(std::string channel_name, std::string mode, std::string senderNickname, int sender_fd, std::string password)
{
	{
		for (size_t x = 0; x < _server.getChannelsList().size(); x++)
		{
			if (_server.getChannelsList()[x]->getName() == channel_name)
			{
				if (mode == "k")
				{
					if (password.empty())
					{
						sendIt(ERR_NEEDMOREPARAMS(senderNickname, "MODE"), sender_fd);
						return;
					}
					addModeK(channel_name, password);
					sendIt("Password set for channel " + channel_name + ": " + password, sender_fd);
					return;
				}
				else
					sendIt(ERR_UNKNOWNMODE(senderNickname, mode), sender_fd);
			}
		}
		sendIt(ERR_NOSUCHCHANNEL(senderNickname, channel_name), sender_fd);
	}
}

void command::mode(const std::string &client_data)
{
	std::istringstream iss(client_data);
	std::string command, channel_name, flag, password;

	iss >> command >> channel_name >> flag >> password;
	std::string senderNickname = getSenderNickname();
	int sender_fd = getSenderFd();

	try
	{
		if (channel_name.empty())
			throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, command));
		if (channel_name[0] != '#')
			throw IrcException("ERR_NOSUCHNICK", ERR_NOSUCHNICK(senderNickname, channel_name));

		if (flag.empty())
		{
			channel* targetChannel = getChannel(channel_name);
			if (!targetChannel)
				throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channel_name));
			std::string currentModes = "Modes actuels du canal " + channel_name;
			sendIt(currentModes, sender_fd);
			return;
		}

		std::string sign = flag.substr(0, 1);
		if (sign != "+" && sign != "-")
			throw IrcException("ERR_UNKNOWNMODE", ERR_UNKNOWNMODE(senderNickname, sign));
			
		std::string mode = flag.substr(1, 1);
		if (mode.empty())
			throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, command));

		if (sign == "+")
			plusSignMode(channel_name, mode, senderNickname, sender_fd, password);
		else
			minusSignMode(channel_name, mode, senderNickname, sender_fd);
	}
	catch (const IrcException& e)
	{
		sendIt(e.getErrorMsg(), sender_fd);
	}
}