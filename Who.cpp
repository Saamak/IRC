#include "includes/command.hpp"

void command::who(const std::string& client_data)
{
    std::istringstream iss(client_data);
    std::string command, target;
    
    iss >> command >> target;
    
    std::string senderNickname = getSenderNickname();
    int sender_fd = getSenderFd();
    
    // Si le canal est spécifié
    if (!target.empty() && isValidChannelName(target))
    {
        channel* targetChannel = getChannel(target);
        if (targetChannel)
        {
            const std::vector<client*>& clients = targetChannel->getClients();
            for (std::vector<client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
            {
                std::string nick = (*it)->getNickname();
                std::string user = (*it)->getUsername();
                if (user.empty()) user = nick; // Fallback si username n'est pas défini
                
                std::string whoReply = ":" + _server.getServerName() + " 352 " + senderNickname + " " + 
                                      target + " " + user + " localhost " + _server.getServerName() + " " + 
                                      nick + " H";
                
                // Ajouter @ si opérateur
                if (targetChannel->IsOperator(nick))
                    whoReply += "@";
                    
                whoReply += " :0 " + user + "\r\n";
                
                send(sender_fd, whoReply.c_str(), whoReply.size(), 0);
            }
        }
    }
    
    // Toujours envoyer END OF WHO
    std::string endOfWho = ":" + _server.getServerName() + " 315 " + senderNickname + " " + 
                           target + " :End of WHO list\r\n";
    send(sender_fd, endOfWho.c_str(), endOfWho.size(), 0);
}