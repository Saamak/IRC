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
#include <cstdlib> 
#include <utility>

void command::mode(const std::string &client_data) 
{
    std::cout << "[DEBUG] Received client_data: " << client_data << std::endl;

    std::istringstream iss(client_data);
    std::string command;
    std::string channel_name;
    std::string flag;
    std::string password;

    iss >> command;
    iss >> channel_name;
    iss >> flag;
    iss >> password; // Récupère le mot de passe si fourni

    std::cout << "[DEBUG] Parsed command: " << command << ", channel_name: " << channel_name 
              << ", flag: " << flag << ", password: " << password << std::endl;

    std::vector<client*>& Client_tmp = _server.getClientList();
    size_t iterator = _server.getIterator() - 1;
    std::string nickname = Client_tmp[iterator]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    std::cout << "[DEBUG] Client nickname: " << nickname << ", fd: " << fd << std::endl;

    if (channel_name.empty() && flag.empty())
    {
        std::cout << "[DEBUG] Missing parameters: channel_name or flag is empty." << std::endl;
        sendIt(ERR_NEEDMOREPARAMS(nickname, command), fd);
        return;
    }
    if (channel_name[0] != '#')
    {
        std::cout << "[DEBUG] Invalid channel name: " << channel_name << std::endl;
        sendIt(ERR_NOSUCHNICK(nickname, channel_name), fd);
        return;
    }

    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    std::cout << "[DEBUG] Number of channels: " << Channel_tmp.size() << std::endl;

    for (size_t x = 0; x < Channel_tmp.size(); x++)
    {
        std::cout << "[DEBUG] Checking channel: " << Channel_tmp[x]->getName() << std::endl;

        if (Channel_tmp[x]->getName() == channel_name) 
        {
            std::cout << "[DEBUG] Found channel: " << channel_name << std::endl;

            if (flag == "+k") // Vérifie si le mode +k est activé
            {
                std::cout << "[DEBUG] Mode +k detected." << std::endl;

                if (password.empty())
                {
                    std::cout << "[DEBUG] Missing password for +k mode." << std::endl;
                    sendIt(ERR_NEEDMOREPARAMS(nickname, command), fd);
                    return;
                }

                Channel_tmp[x]->setChannelFlag(flag); // Active le mode +k
                Channel_tmp[x]->setTopic(password);   // Définit le mot de passe comme "topic" (ou utilisez une méthode dédiée si disponible)
                std::cout << "[DEBUG] Password set for channel " << channel_name << ": " << password << std::endl;

                P << B_G << "Password set for channel " << channel_name << ": " << password << RESET << E;
                sendIt("Password set for channel " + channel_name, fd);
            }
            else
            {
                std::cout << "[DEBUG] Setting channel flag: " << flag << std::endl;
                Channel_tmp[x]->setChannelFlag(flag);
            }
            return;
        }
    }

    std::cout << "[DEBUG] Channel not found: " << channel_name << std::endl;
    sendIt(ERR_NOSUCHCHANNEL(nickname, channel_name), fd);
}