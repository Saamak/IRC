/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:48:11 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 12:48:12 by lvan-slu         ###   ########.fr       */
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

void command::pass(const std::string &client_data) // Pass daniel
{
	std::istringstream iss(client_data);
	std::string command;
	std::string password;
	iss >> command;
	iss >> password;
	std::vector<client*>& Client_tmp = _server.getClientList();
	std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
	int fd = pollfd_tmp[_server.getIterator()].fd;
	size_t iterator = _server.getIterator() - 1;

	if (password.empty()) {
		// Utilisation de sendIrcMessage pour ERR_NEEDMOREPARAMS
		sendIt(ERR_NEEDMOREPARAMS(Client_tmp[iterator]->getNickname(), command), fd);
		return;
	}

	if (Client_tmp[iterator]->getRegistered()) {
		// Utilisation de sendIrcMessage pour ERR_ALREADYREGISTRED
		sendIt(ERR_ALREADYREGISTRED(Client_tmp[iterator]->getNickname()), fd);
		return;
	}

	if (!Client_tmp[iterator]->getRegistered() &&
		(Client_tmp[iterator]->getNickCheck() || Client_tmp[iterator]->getUserCheck())) {
		sendIt(ERR_BADCOMMANDORDER(Client_tmp[iterator]->getNickname()), fd);
		exec("QUIT");
		return;
	}
	Client_tmp[iterator]->setPassCheck(true);
	Client_tmp[iterator]->setClientPassword(password);
}