#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include "colors.h"

class client {
private:
    std::string _nickname;
    bool _passMatch;

public:
    client();
    ~client();
    std::string getNickname();
    void setNickname(std::string nickName);
    bool getMatch();
    void setMatch(bool passMatch);
};

#endif // CLIENT_HPP