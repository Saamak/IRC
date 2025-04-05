
#include "includes/command.hpp"

void command::mode(const std::string &client_data) 
{   ///MODE L
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