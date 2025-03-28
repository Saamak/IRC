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

#include "includes/colors.h"
#include "includes/command.hpp"
#include "includes/channel.hpp"
#include "includes/config.hpp"
#include "IrcException.hpp"
#include "includes/client.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib> 
#include <utility>

void command::invite(const std::string &client_data)
{
    std::istringstream iss(client_data);
    std::string command, targetNickname, channelName;
    
    iss >> command >> targetNickname >> channelName;
    std::string senderNickname = getSenderNickname();
    int sender_fd = getSenderFd();

    try 
	{
        if (channelName[0] != '#' || targetNickname.empty() || channelName.empty())
            throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, "INVITE"));
        
        channel* targetChannel = getChannel(channelName);
        if (!targetChannel)
            throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channelName));
            
        if (!targetChannel->IsInChannel(senderNickname))
            throw IrcException("ERR_NOTONCHANNEL", ERR_NOTONCHANNEL(senderNickname, channelName));
            
        if (targetChannel->IsInChannel(targetNickname))
            throw IrcException("ERR_USERONCHANNEL", ERR_USERONCHANNEL(senderNickname, targetNickname, channelName));
            
        if (!_server.clientExists(targetNickname))
            throw IrcException("ERR_NOSUCHNICK", ERR_NOSUCHNICK(senderNickname, targetNickname));

        int target_fd = _server.getClientFd(targetNickname);
        std::string inviteMessage = ":" + senderNickname + " INVITE " + targetNickname + " " + channelName + "\r\n";
        send(target_fd, inviteMessage.c_str(), inviteMessage.size(), 0);
        
        sendIt(RPL_INVITING(senderNickname, targetNickname, channelName), sender_fd);
    }
    catch (const IrcException& e) 
	{
        sendIt(e.getErrorMsg(), sender_fd);
    }
}

