/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:48:07 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 12:48:07 by lvan-slu         ###   ########.fr       */
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

void command::nick(const std::string &client_data) {

    std::vector<client*>& Client_tmp = _server.getClientList();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;
    size_t iterator = _server.getIterator() - 1;

    std::istringstream iss(client_data);
    std::string command;
    std::string nickname;
    iss >> command;
    iss >> nickname; // Usage: NICK <nickname>, sets your nick6
    if (nickname.empty() && !Client_tmp[iterator]->getRegistered()) 
    {
        sendIt(ERR_NEEDMOREPARAMS(Client_tmp[iterator]->getNickname(), command), fd);
        exec("QUIT");
        return;
    }

    if(Client_tmp[iterator]->getRegistered() && !nickname.empty())
    {
        Client_tmp[iterator]->setNickname(nickname);
        std::string message = "You are now known as " + nickname + "\n";
        send(fd, message.c_str(), message.size(), 0);
        std::vector<channel*> channels_tmp = _server.getChannelsList();
        for (size_t i = 0; i < channels_tmp.size(); i++) 
        {
            std::vector<client*> clients_in_channels = channels_tmp[i]->getClients(); // Fixed here
            for(size_t x = 0; x < clients_in_channels.size(); x++)
            {
                if(clients_in_channels[x]->getNickname() == Client_tmp[iterator]->getNickname())
                {
                    P << "client trouvee dans un channel" << E;
                    sendIt(RPL_NICK())
                }
            }
        }
        return ;
    }
    if(Client_tmp[iterator]->getPassCheck() && Client_tmp[iterator]->getUserCheck() == false && Client_tmp[iterator]->getNickCheck() == false)
    {
        //CHECK SI CE NICK EST DEJA REGISTERED SUR LE SERV, SI OUI , REFUSER LA CONNECTION
        Client_tmp[iterator]->setNickCheck(true);
        Client_tmp[iterator]->setNickname(nickname);
        return ;
    }
    if(Client_tmp[iterator]->getPassCheck() == false || Client_tmp[iterator]->getNickCheck())
    {
        sendIt(ERR_BADCOMMANDORDER(Client_tmp[iterator]->getNickname()), fd);
        exec("QUIT");
        return ;
    }
}

//RPL_NICK


