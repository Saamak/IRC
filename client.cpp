#include "includes/client.hpp"

client::client()
{
    _nickname = "unknown";
    _passMatch = false;
    P <<BOLD<< "Client constructor" <<RESET<< E;
}

client::~client()
{
    P << "Client destructor" << E;
}

std::string client::getNickname()
{
    return (_nickname);
}

void client::setNickname(std::string nickName)
{
    _nickname = nickName;
}

bool client::getMatch()
{
    return (_passMatch);
}

void client::setMatch(bool passMatch)
{
    _passMatch = passMatch;
}




