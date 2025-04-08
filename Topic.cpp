#include "includes/command.hpp"

void command::topic(const std::string &client_data) 
{
    std::string senderNickname = getSenderNickname();
    int sender_fd = getSenderFd();

    try 
    {
        std::istringstream iss(client_data);
        std::string command, channel_name;
        
        iss >> command >> channel_name;
        
        if (channel_name.empty())
            throw IrcException("ERR_NEEDMOREPARAMS", ERR_NEEDMOREPARAMS(senderNickname, "TOPIC"));
            
        if (!isValidChannelName(channel_name))
            throw IrcException("ERR_NOSUCHNICK", ERR_NOSUCHNICK(senderNickname, channel_name));
            
        channel* targetChannel = getChannel(channel_name);
        if (!targetChannel)
            throw IrcException("ERR_NOSUCHCHANNEL", ERR_NOSUCHCHANNEL(senderNickname, channel_name));
            
        if (!targetChannel->IsInChannel(senderNickname))
            throw IrcException("ERR_NOTONCHANNEL", ERR_NOTONCHANNEL(senderNickname, channel_name));
        
        // Extraction du sujet (peut contenir des espaces)
        size_t topicPos = client_data.find(channel_name) + channel_name.length();
        std::string topic_name;
        
        // Chercher si un sujet est fourni
        size_t colonPos = client_data.find(':', topicPos);
        if (colonPos != std::string::npos) {
            topic_name = client_data.substr(colonPos + 1);
        }
        
        // Si aucun sujet n'est fourni, afficher le sujet actuel
        if (topic_name.empty()) 
        {
            if (targetChannel->getTopic().empty())
                sendIt(RPL_NOTOPIC(senderNickname, channel_name), sender_fd);
            else
                sendIt(RPL_TOPIC(senderNickname, channel_name, targetChannel->getTopic()), sender_fd);
        }
        else 
        {
            // Vérifier les restrictions du mode +t
            if (targetChannel->getOpTopic() && !targetChannel->IsOperator(senderNickname))
                throw IrcException("ERR_CHANOPRIVSNEEDED", ERR_CHANOPRIVSNEEDED(senderNickname, channel_name));
            
            // Changer le topic
            targetChannel->setTopic(topic_name);
            
            // Notifier tous les clients du canal
            std::string topicMessage = ":" + senderNickname + " TOPIC " + channel_name + " :" + topic_name + "\r\n";
            std::vector<client*> channelClients = targetChannel->getClients();
            
            for (size_t i = 0; i < channelClients.size(); i++)
            {
                int client_fd = _server.getClientFd(channelClients[i]->getNickname());
                if (client_fd != -1)
                    send(client_fd, topicMessage.c_str(), topicMessage.size(), 0);
            }
            
            std::cout << "Channel " << channel_name << " topic changed to: " << topic_name << std::endl;
        }
    }
    catch (const IrcException& e) {
        sendIt(e.getErrorMsg(), sender_fd);
    }
}