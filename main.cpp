#include "includes/server.hpp"
#include <cstring> // Pour utiliser strcmp
#include "includes/config.hpp"


int check_input(int ac, char **av)
{
    if (ac != 3) {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
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
    P << B_G"\n-----WELCOME TO IRC SERVER-----\n" << RESET << E;
    serv.init(av[2]);
    serv.start();
    serv.myExit();
}