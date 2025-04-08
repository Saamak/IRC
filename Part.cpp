
#include "includes/command.hpp"
#include "includes/command.hpp"
#include "includes/response.hpp"

void command::part(const std::string &client_data)
{
    // Extraction des paramètres
    std::istringstream iss(client_data);
    std::string command, channel_name;
    
    iss >> command >> channel_name;

    P << "Command: " << command << std::endl;
    P << "Channel: " << channel_name << std::endl;
    

    std::string senderNickname = getSenderNickname();
    int sender_fd = getSenderFd();
    
    try {
        // Vérifications similaires à MODE
        if (channel_name.empty())
            throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, "PART"));
        
        if (!isValidChannelName(channel_name))
            throw IrcException("ERR_NOSUCHNICK", ERR_NOSUCHNICK(senderNickname, channel_name));
        
        channel* targetChannel = getChannel(channel_name);
        if (!targetChannel)
            throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channel_name));
        
        if (!targetChannel->IsInChannel(senderNickname))
            throw IrcException("ERR_NOTONCHANNEL", ERR_NOTONCHANNEL(senderNickname, channel_name));
        
        // Création du message PART 
        std::string fullPartMessage = ":" + senderNickname + " PART " + channel_name + " :" + "Leaving" + "\r\n";
        
        // Envoyer le message à tous les membres du canal
        std::vector<client*> channelClients = targetChannel->getClients();
        for (size_t j = 0; j < channelClients.size(); j++) {
            int client_fd = _server.getClientFd(channelClients[j]->getNickname());
            if (client_fd != -1)
                send(client_fd, fullPartMessage.c_str(), fullPartMessage.size(), 0);
        }
        
        // Retirer l'utilisateur du canal
        targetChannel->removeUser(senderNickname);
        
        // Vérifier si le canal est vide et le supprimer si nécessaire
        if (targetChannel->getNumberClient() == 0) {
            std::vector<channel*>& channels = _server.getChannelsList();
            for (size_t i = 0; i < channels.size(); i++) {
                if (channels[i]->getName() == channel_name) {
                    P << "Deleting empty channel: " << channel_name << std::endl;
                    delete channels[i];
                    channels[i] = NULL;
                    channels.erase(channels.begin() + i);
                    break;
                }
            }
        }
        
        std::cout << "User " << senderNickname << " left channel " << channel_name << std::endl;
    }
    catch (const IrcException& e) {
        sendIt(e.getErrorMsg(), sender_fd);
    }
}