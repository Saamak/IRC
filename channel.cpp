#include "includes/channel.hpp"

channel::channel(std::string name): _name(name)
{
    P << "channel constructor" << E;
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

// void channel::removeClient(client* existingClient)
// {
//     _clients.erase(std::remove(_clients.begin(), _clients.end(), existingClient), _clients.end());
// }