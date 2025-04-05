#include "includes/command.hpp"

void command::minusSignMode(std::string channel_name, std::string mode, std::string senderNickname, int sender_fd, std::string argument)
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
                if (argument.empty())
                {
                    sendIt(ERR_NEEDMOREPARAMS(senderNickname, "MODE"), sender_fd);
                    return;
                }
                
                // Vérification que l'émetteur est opérateur (autorisé à retirer le statut)
                if (!targetChannel->IsOperator(senderNickname))
                {
                    sendIt(ERR_CHANOPRIVSNEEDED(senderNickname, channel_name), sender_fd);
                    return;
                }
                
                client* targetClient = _server.getClientByNickname(argument);
                if (!targetClient || !targetChannel->IsInChannel(argument))
                {
                    sendIt(ERR_USERNOTINCHANNEL(senderNickname, argument, channel_name), sender_fd);
                    return;
                }
                
                if (!targetChannel->IsOperator(argument))
                {
                    sendIt("User " + argument + " is not an operator in channel " + channel_name, sender_fd);
                    return;
                }
                
                targetChannel->removeOperator(targetClient);
                
                // Envoyer confirmation à l'émetteur de la commande
                sendIt("User " + argument + " is no longer an operator in channel " + channel_name, sender_fd);
                
                // Envoyer un message MODE IRC standard à tous les membres du canal
                std::string modeMessage = ":" + senderNickname + " MODE " + channel_name + " -o " + argument + "\r\n";
                std::vector<client*> channelClients = targetChannel->getClients();
                
                for (size_t i = 0; i < channelClients.size(); i++)
                {
                    int client_fd = _server.getClientFd(channelClients[i]->getNickname());
                    if (client_fd != -1)
                        send(client_fd, modeMessage.c_str(), modeMessage.size(), 0);
                }
                
                std::cout << "User " << argument << " removed as operator in channel " << channel_name << std::endl;
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