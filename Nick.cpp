#include "includes/command.hpp"

void command::nick(const std::string &client_data) 
{
    std::istringstream iss(client_data);
    std::string command, nickname;
    
    iss >> command >> nickname;
    
    std::string senderNickname = getSenderNickname();
    int sender_fd = getSenderFd();
    
    try 
    {
        std::vector<client*>& clients = _server.getClientList();
        size_t clientIndex = _server.getIterator() - 1;
        
        // Vérification du paramètre nickname
        if (nickname.empty())
            throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, command));
        
        // Vérifier si le pseudonyme est déjà utilisé
        for (size_t i = 0; i < clients.size(); i++) 
        {
            if (clients[i]->getNickname() == nickname && i != clientIndex)
                throw IrcException("ERR_NICKNAMEINUSE", ERR_NICKNAMEINUSE(nickname));
        }
        
        // Si le client est déjà enregistré
        if (clients[clientIndex]->getRegistered())
        {
            std::string oldNickname = clients[clientIndex]->getNickname();
            
            // Trouver tous les canaux où l'utilisateur est présent
            std::vector<channel*> userChannels;
            std::vector<channel*> channels = _server.getChannelsList();
            for (size_t i = 0; i < channels.size(); i++) 
            {
                if (channels[i]->IsInChannel(oldNickname))
                    userChannels.push_back(channels[i]);
            }
            
            // Changer le pseudonyme
            clients[clientIndex]->setNickname(nickname);
            
            // Confirmation au client
            std::string confirmationMessage = ":" + oldNickname + " NICK " + nickname + "\r\n";
            send(sender_fd, confirmationMessage.c_str(), confirmationMessage.size(), 0);
            
            // Notifier tous les utilisateurs des canaux où l'utilisateur est présent
            for (size_t i = 0; i < userChannels.size(); i++) 
            {
                std::vector<client*> channelClients = userChannels[i]->getClients();
                
                for (size_t j = 0; j < channelClients.size(); j++)
                {
                    int client_fd = _server.getClientFd(channelClients[j]->getNickname());
                    // Ne pas envoyer au client qui a changé de pseudonyme
                    if (client_fd != sender_fd && client_fd != -1)
                        send(client_fd, confirmationMessage.c_str(), confirmationMessage.size(), 0);
                }
            }
            return;
        }
        // Client non encore enregistré (phase de connexion)
        else if (clients[clientIndex]->getPassCheck() && !clients[clientIndex]->getUserCheck() && !clients[clientIndex]->getNickCheck())
        {
            clients[clientIndex]->setNickCheck(true);
            clients[clientIndex]->setNickname(nickname);
            return;
        }
        else
            throw IrcException("ERR_BADCOMMANDORDER", ERR_BADCOMMANDORDER(senderNickname));
    }
    catch (const IrcException& e) 
    {
        sendIt(e.getErrorMsg(), sender_fd);
        
        // Si l'erreur est grave, déconnecter le client
        if (e.getErrorCode() == "ERR_BADCOMMANDORDER")
            exec("QUIT");
    }
}


