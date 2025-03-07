#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include "colors.h"

class client {
private:
    std::string _nickname;
    std::string _username;
    bool _passMatch;

public:
    client();
    ~client();
    std::string getNickname();
    void setNickname(std::string nickName);
    bool getMatch();
    void setMatch(bool passMatch);
    void setUsername(std::string userName);
    std::string getUsername();
};

#endif // CLIENT_HPP