#include "server.hpp"

int check_input(int ac, char **av)
{
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <port>" << std::endl;
        return 1;
    }
    
    int port = atoi(av[1]);
    if (port < 1024 || port > 65535) {
        std::cerr << "Error: Port must be between 1024 and 65535" << std::endl;
        return 1;
    }

    return port;
}

int main(int ac, char **av){

    int port = check_input(ac, av);
    if (port == 1)
        exit(1);
    Server serv(port);
    P << B_G"\n-----WECLOME TO IRC SERVER-----\n" << RESET << E;
    serv.init();
    serv.start();
    serv.stop();
}