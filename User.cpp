/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:48:17 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 12:48:18 by lvan-slu         ###   ########.fr       */
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

void command::user(const std::string &client_data) 
{
    std::vector<client*>& Client_tmp = _server.getClientList();
    size_t iterator = _server.getIterator() - 1;
    std::istringstream iss(client_data);
    std::string command;
    std::string username;
    std::string nothing = "Unknown";
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();


    int fd = pollfd_tmp[_server.getIterator()].fd;
    iss >> command;
    iss >> username;
    if (username.empty())
    {
        sendIt(ERR_NEEDMOREPARAMS(nothing, command), fd);
        exec("QUIT");
        return ;
    }
    if(Client_tmp[iterator]->getRegistered())
    {
        std::string nickname = Client_tmp[iterator]->getNickname();
        sendIt(ERR_ALREADYREGISTRED(nickname), fd);
        return ;
    }
    if (Client_tmp[iterator]->getPassCheck() == false || Client_tmp[iterator]->getNickCheck() == false)
    {
        std::string message = "Commands bad order during connection, PASS/NICK/USER\nClient Disconnected\n";
        send(fd, message.c_str(), message.size(), 0);
        exec("QUIT");
        return ;
    }
    if (Client_tmp[iterator]->getPassCheck() && Client_tmp[iterator]->getNickCheck())
    {
        Client_tmp[iterator]->setUserCheck(true);
        Client_tmp[iterator]->setUsername(username);
        if (Client_tmp[iterator]->getClientPassword() == _server.getPassword())
        {
            std::string nickname = Client_tmp[iterator]->getNickname();
            sendIt(RPL_WELCOME(nickname), fd);
            Client_tmp[iterator]->setRegistered(true);
        }   
        else
        {
            std::string message = "Wrong Password\nClient Disconnected\n";
            send(fd, message.c_str(), message.size(), 0);
            exec("QUIT");
        }
        return ;
    }
}
