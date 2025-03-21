#include "includes/channel.hpp"

channel::channel(std::string name): _name(name)
{
    P << "channel constructor" << E;
    isInvitOnly = false;
    isPasswd = false;
    opTopic = false;
    maxUser = std::numeric_limits<size_t>::max();
}

channel::~channel()
{
    P << "destructor channel" << E;
}
std::string channel::getName() const
{
    return _name;
}

void channel::addClient(client* newClient)
{
    client_lst.push_back(newClient);
}

bool    channel::IsInChannel(const std::string &name)
{
    for (size_t x = 0; x < client_lst.size(); x++)
    {
        if (client_lst[x]->getNickname() == name)
            return true;
    }
    return false;
}

void channel::addOperator(client * newOperator)
{
   operator_lst.push_back(newOperator);
}

std::vector<client*> channel::getClients() const {
    return client_lst;
}


std::string channel::getTopic() const
{
    return (_topic);
}

void channel::setTopic(const std::string topic_name)
{
    _topic = topic_name;
}


void    channel::setChannelFlag(std::string flag)
{
    P << flag.size() << E;
    if(flag.size() > 2)
    {
        P << B_R <<"erreur flag DE MERDE" << E;
        return ;
    }
    if (flag[0] != '-' || flag[0] != '+')
    {
        P << B_R <<"+ - ERROR" << E;
        return ;
    }

    switch (flag[1]) 
    {
        case 'i':
            isInvitOnly = !isInvitOnly;
            P << B_Y << "InvitObly set to : " << getOpTopic() << E;
            break;
        case 't':
            opTopic = !opTopic;
            P << B_Y << "op topic set to : " << getOpTopic() << E;
            break;
        case 'k':
            isPasswd = !isPasswd;
            P << B_Y << "KeyMode set to : " << getOpTopic() << E;
            break;
        case 'o':
            P<< "flag" << B_Y << " o " <<RESET << "non pris en charge pour le moment" << E;
            break;
        case 'l':
            P << "flag" << B_Y << " l " <<RESET << "non pris en charge pour le moment" << E;
            break;
        default:
            P << B_R << "Flag inconnu" << E;
            break;
    }
}

bool channel::getIsInvitOnly() const {
    return isInvitOnly;
}

void channel::setIsInvitOnly(bool value) {
    isInvitOnly = value;
}

bool channel::getOpTopic() const {
    return opTopic;
}

void channel::setOpTopic(bool value) {
    opTopic = value;
}

bool channel::getIsPasswd() const {
    return isPasswd;
}

void channel::setIsPasswd(bool value) {
    isPasswd = value;
}