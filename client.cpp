#include "includes/client.hpp"

client::client()
{
    _nickname = "unknown";
    _username = "unknown";
    _clientPassword = "unknown";
    _isRegistered = false;
    _passCheck = false;
    _nickCheck = false;
    _userCheck = false;
    P <<BOLD<< "Client constructor" <<RESET<< E;
}

client::~client()
{
    P << "Client destructor" << E;
}

std::string client::getBufferClient()
{
    return (_buffer_client);
}

void client::setBufferClient(std::string buffer_client)
{
    _buffer_client = buffer_client;
}

void client::emptyBufferClient()
{
    _buffer_client.clear();
}

std::string client::getNickname()
{
    return (_nickname);
}

void client::setNickname(std::string nickName)
{
    _nickname = nickName;
}

void client::setUsername(std::string userName)
{
    _username = userName;
}

std::string client::getUsername()
{
    return (_username);
}

bool client::getRegistered(){
    return _isRegistered;
}

bool client::getPassCheck(){
    return _passCheck;
}

bool client::getNickCheck(){
    return _nickCheck;
}

bool client::getUserCheck(){
    return _userCheck;
}

void client::setRegistered(bool check){
    _isRegistered = check;
}

void client::setPassCheck(bool check){
    _passCheck = check;
}

void client::setNickCheck(bool check){
    _nickCheck = check;
}

void client::setUserCheck(bool check){
    _userCheck = check;
}

std::string client::getClientPassword()
{
    return _clientPassword;
}

void    client::setClientPassword(std::string password)
{
    _clientPassword = password;
}
