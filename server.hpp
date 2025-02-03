#ifndef SERVER_HPP
#define SERVER_HPP

#include "colors.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <poll.h>

class Server {
private:
    int _port;
    int _server_fd;
    struct sockaddr_in _server_addr;
    std::vector<struct pollfd> _poll_fds; // va contenir tout les fds.
    // map/vector de classe client
    // map/vector de classe channel

public:
    Server(int port);
    ~Server();
    int getPort() const { return _port; }
    bool init();
    void start();
    void stop();
    void integrity(std::string client_data); // Ajout de la déclaration de la méthode integrity
};

#endif // SERVER_HPP