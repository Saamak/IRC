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
    iss >> nickname;
    P << nickname << E;
    if (nickname.empty()) {
        sendIt(ERR_NEEDMOREPARAMS(Client_tmp[iterator]->getNickname(), command), fd);
        exec("QUIT");
        return;
    }
    if(Client_tmp[iterator]->getRegistered())
    {
        //CHECK SI CE NICK EST DEJA REGISTERED SUR LE SERV, SI OUI , REFUSER LE NOUVEAU NICK, DONC LAISSER L'ACTUEl
        Client_tmp[iterator]->setNickname(nickname);
        std::string message = "You are now known as " + nickname + "\n";
        send(fd, message.c_str(), message.size(), 0);
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
