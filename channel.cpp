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

// void channel::addClient(client* newClient)
// {
//     _clients.push_back(newClient);
// }

// void channel::removeClient(client* existingClient)
// {
//     _clients.erase(std::remove(_clients.begin(), _clients.end(), existingClient), _clients.end());
// }