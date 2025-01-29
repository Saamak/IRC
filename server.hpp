#include "colors.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <poll.h> 

class Server{
    private :
        int _port;
        int _server_fd;
        struct sockaddr_in _server_addr;
        std::vector<struct pollfd> _poll_fds; // va contenir tout les fds.

    public :
        Server(int port);;
        ~Server();
        int getPort()const {return _port;}
        bool init();
        void start();
        void stop();

};