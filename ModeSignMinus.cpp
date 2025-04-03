/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeSignMinus.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 13:58:08 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/28 13:58:08 by lvan-slu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/colors.h"
#include "includes/command.hpp"
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "includes/IrcException.hpp"
#include "includes/client.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib> 
#include <utility>

void command::minusSignMode(std::string channel_name, std::string mode, std::string senderNickname, int sender_fd)
{
    for (size_t x = 0; x < _server.getChannelsList().size(); x++)
    {
        if (_server.getChannelsList()[x]->getName() == channel_name)
        {
            channel* targetChannel = _server.getChannelsList()[x];

            if (mode == "k") // Gestion du flag -k (supprimer la clé)
            {
                targetChannel->setIsPasswd(false);
                targetChannel->setKey(""); // Supprime la clé
                sendIt("Password removed for channel " + channel_name, sender_fd);
                std::cout << "Channel " << channel_name << " password protection disabled: " << targetChannel->getIsPasswd() << std::endl;
                return;
            }
            else if (mode == "o") // Gestion du flag -o (retirer un opérateur)
            {
                client* targetClient = _server.getClientByNickname(senderNickname);
                if (!targetClient || !targetChannel->IsOperator(senderNickname))
                {
                    sendIt(ERR_USERNOTINCHANNEL(senderNickname, senderNickname, channel_name), sender_fd);
                    return;
                }
                targetChannel->removeOperator(targetClient);
                sendIt("User " + senderNickname + " is no longer an operator in channel " + channel_name, sender_fd);
                std::cout << "User " << senderNickname << " removed as operator in channel " << channel_name << std::endl;
                return;
            }
            else if (mode == "i") // Gestion du flag -i (supprimer l'invitation uniquement)
            {
                targetChannel->setIsInvitOnly(false);
                sendIt("Channel " + channel_name + " is no longer invite-only", sender_fd);
                std::cout << "Channel " << channel_name << " invite-only mode disabled: " << targetChannel->getIsInvitOnly() << std::endl;
                return;
            }
            else if (mode == "t") // Gestion du flag -t (supprimer la restriction de sujet)
            {
                targetChannel->setOpTopic(false);
                sendIt("Topic restriction disabled for channel " + channel_name, sender_fd);
                std::cout << "Channel " << channel_name << " topic restriction disabled: " << targetChannel->getOpTopic() << std::endl;
                return;
            }
            else if (mode == "l") // Gestion du flag -l (supprimer la limite d'utilisateurs)
            {
                targetChannel->setLimit(std::numeric_limits<size_t>::max());
                sendIt("User limit removed for channel " + channel_name, sender_fd);
                std::cout << "Channel " << channel_name << " user limit removed" << std::endl;
                return;
            }
            else
            {
                sendIt(ERR_UNKNOWNMODE(senderNickname, mode), sender_fd);
                return;
            }
        }
    }
    sendIt(ERR_NOSUCHCHANNEL(senderNickname, channel_name), sender_fd);
}