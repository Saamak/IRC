#include "includes/client.hpp"

client::client()
{
    P << "Client constructor" << E;
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




