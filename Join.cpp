/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvan-slu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:30:09 by lvan-slu          #+#    #+#             */
/*   Updated: 2025/03/26 11:30:10 by lvan-slu         ###   ########.fr       */
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

void command::join(const std::string &client_data) 
{
    std::cout << "[DEBUG] Processing JOIN command: " << client_data << std::endl;
     // 1. Parse la commande pour extraire le nom du canal et le mot de passe
    std::istringstream iss(client_data);
    std::string command;
    std::string channel_name;
    std::string password;

    iss >> command; // recup commande
    iss >> channel_name; // recup channel
    iss >> password; // Récupère le mot de passe si fourni

    // 2. Validation de base
    if (channel_name.empty()) {
        std::cout << "[DEBUG] Missing channel name" << std::endl;
        return;
    }
    
    if (channel_name[0] != '#') {
        std::cout << "[DEBUG] Invalid channel name format: " << channel_name << std::endl;
        return;
    }

    // 3. Récupère les informations nécessaires
    std::vector<channel*>& Channel_tmp = _server.getChannelsList();
    std::vector<client*>& Client_tmp = _server.getClientList();
    size_t iterator = _server.getIterator() - 1;
    std::string nickname = Client_tmp[iterator]->getNickname();
    std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
    int fd = pollfd_tmp[_server.getIterator()].fd;

    std::cout << "[DEBUG] Checking for existing channel: " << channel_name << std::endl;
    
    // 4. Recherche si le canal existe 
    
    bool channelExists = false;
    for (size_t x = 0; x < Channel_tmp.size(); x++) {
        if (Channel_tmp[x]->getName() == channel_name) 
        {
            channelExists = true;
            std::cout << "[DEBUG] Channel exists: " << channel_name << std::endl;
            
             // 5a. Vérifie les restrictions (invite-only, mot de passe)
            if (Channel_tmp[x]->getIsInvitOnly() && !Channel_tmp[x]->isInvited(nickname)) {
                std::cout << "[DEBUG] Channel is invite-only: " << channel_name << std::endl;
                sendIt(ERR_INVITEONLYCHAN(nickname, channel_name), fd);
                return;
            }
            
            // Vérifie si le canal est protégé par un mot de passe
            if (Channel_tmp[x]->getIsPasswd()) {
                std::string channel_password = Channel_tmp[x]->getKey(); // Utilisez le topic comme mot de passe
                std::cout << "[DEBUG] Channel is password-protected. Expected: '" 
                << channel_password << "', Received: '" << password << "'" << std::endl;
                
                if (password.empty() || channel_password != password) {
                    std::cout << "[DEBUG] Incorrect or missing password for channel: " << channel_name << std::endl;
                    sendIt(ERR_BADCHANNELKEY(nickname, channel_name), fd);
                    return;
                } else {
                    std::cout << "[DEBUG] Password accepted for channel: " << channel_name << std::endl;
                }
            }
            
            if (!Channel_tmp[x]->IsInChannel(nickname)) 
            {
                std::cout << "[DEBUG] Adding client to existing channel: " << channel_name << std::endl;
                Channel_tmp[x]->addClient(Client_tmp[iterator]);
                
                // Envoyer les messages de confirmation
                std::string join_message = ":" + nickname + " JOIN " + channel_name + "\r\n";
                send(fd, join_message.c_str(), join_message.size(), 0);
                
                // Envoyer le topic
                if (!Channel_tmp[x]->getTopic().empty() && !Channel_tmp[x]->getIsPasswd()) {
                    sendIt(RPL_TOPIC(nickname, channel_name, Channel_tmp[x]->getTopic()), fd);
                } else {
                    sendIt(RPL_NOTOPIC(nickname, channel_name), fd);
                }
                
                // AJOUT: Générer la liste des utilisateurs dans le canal
                std::string users_list = "";
                const std::vector<client*>& channel_clients = Channel_tmp[x]->getClients();
                for (size_t i = 0; i < channel_clients.size(); i++) {
                    if (i > 0) users_list += " ";
                    users_list += channel_clients[i]->getNickname();
                }
                //retirer de la liste d invite
                
                // AJOUT: Envoyer RPL_NAMREPLY (liste des utilisateurs)
                sendIt(RPL_NAMREPLY(nickname, channel_name, users_list), fd);
                
                // AJOUT: Envoyer RPL_ENDOFNAMES (fin de la liste)
                sendIt(RPL_ENDOFNAMES(nickname, channel_name), fd);
            } else {
                std::cout << "[DEBUG] Client already in channel: " << channel_name << std::endl;
            }
            return;
        }
    }

    std::cout << "[DEBUG] Creating new channel: " << channel_name << std::endl;
    
    // Si le canal n'existe pas, créez-le
    if (!channelExists) {
        channel* new_channel = new channel(channel_name);
        new_channel->addClient(Client_tmp[iterator]);
        new_channel->addOperator(Client_tmp[iterator]);

        // Si un mot de passe est fourni, configurez le canal pour qu'il soit protégé par un mot de passe
        if (!password.empty()) {
            new_channel->setIsPasswd(true);
            new_channel->setTopic(password); // Définit le mot de passe comme "topic"
            std::cout << "[DEBUG] Password set for new channel: " << channel_name << ", password: " << password << std::endl;
        }

        _server.addChannel(new_channel);
        
        // Envoyer les messages de confirmation
        std::string join_message = ":" + nickname + " JOIN " + channel_name + "\r\n";
        send(fd, join_message.c_str(), join_message.size(), 0);
        
        // Envoyer le topic par défaut pour le nouveau canal
        std::string welcome_topic = "Welcome to the channel!";
        if (!new_channel->getIsPasswd()) {
            new_channel->setTopic(welcome_topic);
            sendIt(RPL_TOPIC(nickname, channel_name, welcome_topic), fd);
        } else {
            sendIt(RPL_NOTOPIC(nickname, channel_name), fd);
        }
        
        // AJOUT: Envoyer RPL_NAMREPLY (liste des utilisateurs - uniquement le créateur pour l'instant)
        sendIt(RPL_NAMREPLY(nickname, channel_name, nickname), fd);
        
        // AJOUT: Envoyer RPL_ENDOFNAMES (fin de la liste)
        sendIt(RPL_ENDOFNAMES(nickname, channel_name), fd);
        
        std::cout << "[DEBUG] Channel created and client added as operator: " << channel_name << std::endl;
    }

    _server.printChannelsAndClients();
}