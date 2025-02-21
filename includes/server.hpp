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

class channel;

class Server {
private:
    int _port;
    int _server_fd;
    struct sockaddr_in _server_addr;
    std::vector<struct pollfd> _poll_fds; // va contenir tout les fds.
    std::vector<channel*> channels_lst;
    std::vector<client*> client_lst; // Vecteur de clients
    client* newClient; // Ajouter un pointeur vers un objet client

public:
    std::vector<std::string> tokens;
    Server(int port);
    ~Server();
    int getPort() const { return _port; }
    bool init();
    void start();
    void stop();
    void integrity(std::string client_data);
    void addChannel(channel* new_channel);
    void clientConnected(int i);
    int HandleCommunication(int i);
    void addClient(client* newClient);
    void removeClient(client* existingClient);
    client* getNewClient() const;
    void setNewClient(client* client);
};

#endif // SERVER_HPP