#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include "colors.h"

class client {
private:
    std::string _nickname;
    std::string _username;
    std::string _clientPassword;
    std::string _buffer_client;
    bool _isRegistered;
    bool _passCheck;
    bool _nickCheck;
    bool _userCheck;

public:
    client();
    ~client();
    std::string getNickname();
    void setBufferClient(std::string buffer_client);
    std::string getBufferClient(void);
    void emptyBufferClient();
    void setNickname(std::string nickName);
    void setUsername(std::string userName);
    bool getRegistered();
    bool getPassCheck();
    bool getNickCheck();
    bool getUserCheck();
    std::string getClientPassword();
    void setRegistered(bool check);
    void setPassCheck(bool check);
    void setNickCheck(bool check);
    void setUserCheck(bool check);
    void setClientPassword(std::string password);
    std::string getUsername();
};

#endif // CLIENT_HPP