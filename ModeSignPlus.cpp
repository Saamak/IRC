/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeSignPlus.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 13:58:04 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/28 13:58:05 by lvan-slu         ###   ########.fr       */
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

void command::plusSignMode(std::string channel_name, std::string mode, std::string senderNickname, int sender_fd, std::string argument)
{
    for (size_t x = 0; x < _server.getChannelsList().size(); x++)
    {
        if (_server.getChannelsList()[x]->getName() == channel_name)
        {
            channel* targetChannel = _server.getChannelsList()[x];

            if (mode == "k") // Gestion du flag +k (clé/mot de passe)
            {
                if (argument.empty())
                {
                    sendIt(ERR_NEEDMOREPARAMS(senderNickname, "MODE"), sender_fd);
                    return;
                }
                targetChannel->setIsPasswd(true);
                targetChannel->setKey(argument);
                sendIt("Password set for channel " + channel_name + ": " + argument, sender_fd);
                std::cout << "Channel " << channel_name << " password protection enabled: " << targetChannel->getIsPasswd() << std::endl;
                return;
            }
            else if (mode == "o") // Gestion du flag +o (opérateur)
            {
                if (argument.empty())
                {
                    sendIt(ERR_NEEDMOREPARAMS(senderNickname, "MODE"), sender_fd);
                    return;
                }
                client* targetClient = _server.getClientByNickname(argument);
                if (!targetClient || !targetChannel->IsInChannel(argument))
                {
                    sendIt(ERR_USERNOTINCHANNEL(senderNickname, argument, channel_name), sender_fd);
                    return;
                }
                targetChannel->addOperator(targetClient);
                sendIt("User " + argument + " is now an operator in channel " + channel_name, sender_fd);
                std::cout << "User " << argument << " added as operator in channel " << channel_name << std::endl;
                return;
            }
            else if (mode == "i") // Gestion du flag +i (invitation uniquement)
            {
                targetChannel->setIsInvitOnly(true);
                sendIt("Channel " + channel_name + " is now invite-only", sender_fd);
                std::cout << "Channel " << channel_name << " invite-only mode enabled: " << targetChannel->getIsInvitOnly() << std::endl;
                return;
            }
            else if (mode == "t") // Gestion du flag +t (restriction de sujet)
            {
                targetChannel->setOpTopic(true);
                sendIt("Topic restriction enabled for channel " + channel_name, sender_fd);
                std::cout << "Channel " << channel_name << " topic restriction enabled: " << targetChannel->getOpTopic() << std::endl;
                return;
            }
            else if (mode == "l") // Gestion du flag +l (limite d'utilisateurs)
            {
                if (argument.empty())
                {
                    sendIt(ERR_NEEDMOREPARAMS(senderNickname, "MODE"), sender_fd);
                    return;
                }
                int limit = std::stoi(argument);
                targetChannel->setLimit(limit);
                sendIt("User limit set for channel " + channel_name + ": " + argument, sender_fd);
                std::cout << "Channel " << channel_name << " user limit set to: " << limit << std::endl;
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