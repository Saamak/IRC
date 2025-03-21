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
    if(flag.size() > 2 || (flag[0] != '-' || flag[0] != '+'))
    {
        P << B_R <<"erreur flag DE MERDE" << E;
        return ;
    }
}