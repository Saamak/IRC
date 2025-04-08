
#include "includes/command.hpp"

void command::createChannel(const std::string& channel_name, const std::string& password, const std::string& senderNickname, int sender_fd)
{
    channel* newChannel = new channel(channel_name);
    newChannel->addClient(getSender());
    newChannel->addOperator(getSender());
    newChannel->setOpTopic(true);

    if (!password.empty())
    {
        newChannel->setIsPasswd(true);
        newChannel->setKey(password);
    }

    _server.addChannel(newChannel);

    // 1. Envoyer JOIN en premier
    std::string joinMessage = ":" + senderNickname + " JOIN " + channel_name + "\r\n";
    send(sender_fd, joinMessage.c_str(), joinMessage.size(), 0);

    // 3. Envoyer NAMREPLY et ENDOFNAMES
    std::string usersList = "@" + senderNickname;
    sendIt(RPL_NAMREPLY(senderNickname, channel_name, usersList), sender_fd);
    sendIt(RPL_ENDOFNAMES(senderNickname, channel_name), sender_fd);
}

void command::join(const std::string& client_data)
{
    std::istringstream iss(client_data);
    std::string command, channel_name, password;
    
    iss >> command >> channel_name >> password;

    std::string senderNickname = getSenderNickname();
    int sender_fd = getSenderFd();
    
    try
    {
        if (channel_name.empty())
            throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, "JOIN"));
        if (channel_name[0] != '#')
            throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channel_name));
        
        channel* targetChannel = getChannel(channel_name);
        std::cout << channel_name << senderNickname << std::endl;
        
        if (targetChannel)
        {
            // Vérifications des conditions d'accès au canal
            if (targetChannel->getIsInvitOnly() && !targetChannel->isInvited(senderNickname))
                throw IrcException("ERR_INVITEONLYCHAN", ERR_INVITEONLYCHAN(senderNickname, channel_name));
            if (targetChannel->getIsPasswd() && targetChannel->getKey() != password)
                throw IrcException("ERR_BADCHANNELKEY", ERR_BADCHANNELKEY(senderNickname, channel_name));
            if (targetChannel->IsInChannel(senderNickname))
                throw IrcException("ERR_USERONCHANNEL", ERR_USERONCHANNEL(senderNickname, channel_name, senderNickname));
            if (targetChannel->getLimit() != std::numeric_limits<size_t>::max() && targetChannel->getClients().size() >= targetChannel->getLimit())
                throw IrcException("ERR_CHANNELISFULL", ERR_CHANNELISFULL(senderNickname, channel_name));
            
            // Ajouter le client au canal AVANT de construire la liste
            targetChannel->addClient(getSender());
            
            // Créer le message JOIN
            std::string joinMessage = ":" + senderNickname + " JOIN " + channel_name + "\r\n";
            
            // IMPORTANT: Diffuser le message JOIN à TOUS les clients du canal y compris le nouveau
            const std::vector<client*>& clients = targetChannel->getClients();
            for (std::vector<client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
            {
                int client_fd = _server.getClientFd((*it)->getNickname());
                if (client_fd != -1)
                    send(client_fd, joinMessage.c_str(), joinMessage.size(), 0);
            }
            
            // Envoyer les informations du canal
            if (!targetChannel->getTopic().empty())
                sendIt(RPL_TOPIC(senderNickname, channel_name, targetChannel->getTopic()), sender_fd);
            else
                sendIt(RPL_NOTOPIC(senderNickname, channel_name), sender_fd);

            // Construire la liste des utilisateurs avec préfixes pour les opérateurs
            std::string usersList;
            for (std::vector<client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
            {
                if (!usersList.empty())
                    usersList += " ";
                    
                // Ajouter le préfixe @ pour les opérateurs
                if (targetChannel->IsOperator((*it)->getNickname()))
                    usersList += "@";
                    
                usersList += (*it)->getNickname();
            }
            // Envoyer la liste complète d'utilisateurs au nouveau client
            sendIt(RPL_NAMREPLY(senderNickname, channel_name, usersList), sender_fd);
            sendIt(RPL_ENDOFNAMES(senderNickname, channel_name), sender_fd);
            
            // Si le client était invité, le supprimer de la liste d'invitation
            if (targetChannel->isInvited(senderNickname))
                targetChannel->removeInvite(senderNickname);
            return;
        }
        // Si le canal n'existe pas, le créer
        createChannel(channel_name, password, senderNickname, sender_fd);
    }
    catch (const IrcException& e)
    {
        sendIt(e.getErrorMsg(), sender_fd);
    }
}