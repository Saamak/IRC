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


void command::mode(const std::string &client_data)    ///MODE L
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

    channel* targetChannel = getChannel(channel_name);
    if (!targetChannel)
        throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channel_name));

    if (flag.empty())
    {
        std::string modeStr = targetChannel->getModes();
        if (targetChannel->getIsPasswd())
            modeStr += " " + targetChannel->getKey();
    
        sendIt(RPL_CHANNELMODEIS(senderNickname, channel_name, modeStr), sender_fd);
        return;
    }

    // Vérifiez si l'utilisateur est un opérateur
    if (!targetChannel->IsOperator(senderNickname))
        throw IrcException("ERR_CHANOPRIVSNEEDED", ERR_CHANOPRIVSNEEDED(senderNickname, channel_name));

    // analyser les paramètres
    std::vector<std::pair<std::string, std::string> > arguments = parsing_param_mode(client_data);

    // Parcourir les arguments analysés
    for (size_t i = 0; i < arguments.size(); ++i)
    {
        std::string sign = arguments[i].first;
        std::string mode = arguments[i].second;
		std::cout << B_R << "Processing mode: " << mode << " with sign: " << sign << std::endl;

        if (sign == "+")
            plusSignMode(channel_name, mode, senderNickname, sender_fd, password);
        else if (sign == "-")
            minusSignMode(channel_name, mode, senderNickname, sender_fd);
        else
            throw IrcException("ERR_UNKNOWNMODE", ERR_UNKNOWNMODE(senderNickname, sign));
    }
}
	catch (const IrcException& e)
	{
		sendIt(e.getErrorMsg(), sender_fd);
	}
}