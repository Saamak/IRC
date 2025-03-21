#ifndef SERVER_HPP
#define SERVER_HPP

#include "colors.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <poll.h>
#include "channel.hpp"
#include "client.hpp"
#include "response.hpp"

class channel;

class Server {
private:
    std::string _server_name;
    int iterator;
    int _port;
    int _server_fd;
    bool _exit;
    std::string _password;
    struct sockaddr_in _server_addr;
    std::vector<struct pollfd> _poll_fds; // va contenir tout les fds.
    std::vector<channel*> channels_lst;
    std::vector<client*> client_lst; // Vecteur de clients
    client* newClient; // Ajouter un pointeur vers un objet client

public:
    std::vector<std::string> tokens;
    Server(int port);
    ~Server();
    bool init(char *pass);
    void start();
    void stop();
    void integrity(std::string client_data);
    void addChannel(channel* new_channel);
    void clientConnected();
    int HandleCommunication(int i);
    void addClient(client* newClient);
    void removeClient(client* existingClient);
    void printChannelsAndClients() const;
    void sendToClient(int client_fd, const std::string &message);

    //SETTER GETTER
    int getPort() const { return _port; }
    void    setBoolExit(bool tmp);
    client* getNewClient() const;
    std::vector<channel *>& getChannelsList();
    std::vector<client*>& getClientList();
    std::vector<struct pollfd>& getPollFd(); 
    int getIterator();
    void setIterator(int i);
    int getSizeClientList() const;
    client* getClientList(int x) const;
    void setNewClient(client* client);
    std::string getPassword();
    std::string getServerName();

    void sendIrcMessage(const std::string& server_name, const std::string& code, const std::string& nickname, const std::string& channel_name, const std::string& additional_info, int fd);
};

#endif // SERVER_HPP