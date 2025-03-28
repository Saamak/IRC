#include "includes/server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "includes/command.hpp"
#include <algorithm> 
#include <csignal>

bool exit_b = false;

Server::Server(int port) : _port(port), _server_fd(-1), newClient(NULL) {
    P <<BOLD <<"SERVEUR CONSTRUCTOR" <<RESET <<E;
    std::memset(&_server_addr, 0, sizeof(_server_addr));
}

Server::~Server() {
    P <<BOLD <<"SERVEUR DESTRUCTOR" <<RESET <<E;
    if (_server_fd != -1)
        close(_server_fd);
}

void Server::integrity(std::string client_data) {
    command cmdd(*this); // Passe une référence au serveur
    cmdd.exec(client_data);
}

void Server::myExit() {
    // delete(getNewClient());
    // setNewClient(NULL); // Réinitialiser le pointeur

    for (size_t i = 0; i < client_lst.size(); ++i) {
        if (client_lst[i] != NULL) { // Vérifier si le pointeur est valide
            delete client_lst[i];
            client_lst[i] = NULL; // Mettre le pointeur à NULL après suppression
        }
    }
    client_lst.clear();
    _poll_fds.clear();
    clearChannels();
}

bool Server::init(char *pass)
{
    _server_name = "German Elite V2";
    _password = pass;
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
    {
        std::cerr << "Error: socket creation failed" << std::endl;
        return false;
    }

    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    _server_addr.sin_port = htons(_port);

    if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) < 0)
    {
        std::cerr << "Error: bind failed" << std::endl;
        close(_server_fd);
        return false;
    }

    return true;
}

// Fonction pour diviser une chaîne de caractères par un délimiteur donné
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void Server::clientConnected()
{
    int client_socket = accept(_server_fd, NULL, NULL);
    if (client_socket < 0)
    {
        std::cerr << "Error: accept failed" << std::endl;
        return ;
    }
    //check USER/NICK
    std::cout << G"Client connected SAS\n" << RESET << std::endl;

    struct pollfd client_pollfd;
    client_pollfd.fd = client_socket;
    client_pollfd.events = POLLIN;
    client_pollfd.revents = 0;
    _poll_fds.push_back(client_pollfd);

    newClient = new client();
    addClient(newClient);

}

int Server::HandleCommunication(int i)
{
    // Handle client communication
    char buffer[1024];
    int bytes_read = read(_poll_fds[i].fd, buffer, sizeof(buffer));
    buffer[bytes_read] = '\0';
    if (buffer[0] == '\0')
    {
        integrity("QUIT");
        return (i);
    }
    std::string client_test(buffer);
    std::string buffer_client_tmp = client_lst[iterator - 1]->getBufferClient();
    if (buffer_client_tmp.empty() == false)
    {
        client_test = buffer_client_tmp + client_test;
    }

    if (client_test.find('\n') == (size_t)-1)
    {
        client_lst[iterator - 1]->setBufferClient(client_test);
        return (i);
    }
    // IF \r\n detecter , lancer la commande, si /r/n non detecter , ne pas lancer la commande, stocker dans un buffer jusqu'a ce que la commande finale soit envoyee.
    //Attention, dans la pratique peut etre faire une comparaison entre buffer + chaine recu , car le terminal renvoie a chaque fois l'integralitie de la commande.
    // Voir exemple sujet, renvoie com, pui com man, puis com man, puis com man \r\n.
    if (bytes_read < 0) 
    {
        std::string message =  "READ ERROR in client fd\nClient Disconnected\n";
        send(_poll_fds[iterator - 1].fd, message.c_str(), message.size(), 0);
        integrity("QUIT");
    }
    else
    {
        client_lst[iterator - 1]->emptyBufferClient();
        std::vector<std::string> splitted = split(client_test, '\n');
        for(size_t x = 0; x < splitted.size(); x++)
        {
            P << B_Y << splitted[x] << RESET << E;
            integrity(splitted[x]);
        }
    }
    return (i);
}

void handleSignal(int signal)
{
    if(signal == SIGINT)
    {
        std::cout << BOLD << "SIGINT Received" << E;
        exit_b = true;
    }
}

void Server::start()
{
    if (listen(_server_fd, 5) < 0)
    {
        std::cerr << "Error: listen failed" << std::endl;
        return;
    }
    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    _poll_fds.push_back(server_pollfd);

    while (true)
    {
        signal(SIGINT, handleSignal);
        if(exit_b == true)
        {
            P <<B_Y << "SERVER OFF" << E;
            myExit();
        }
        int poll_count = poll(_poll_fds.data(), _poll_fds.size(), -1);
        if (poll_count < 0)
        {
            std::cerr << "Error: poll failed" << std::endl;
            return;
        }
        for (size_t i = 0; i < _poll_fds.size(); ++i)
        {
            iterator = i;
            if (_poll_fds[i].revents & POLLIN)
            {
                if (_poll_fds[i].fd == _server_fd)
                {
                    clientConnected();
                } 
                else 
                {
                    i = HandleCommunication(i);
                }
            }
        }
        P << B_Y "Client_lst size : " << client_lst.size() << RESET << E;
        P << B_Y "Poll_fd size : " << _poll_fds.size() << RESET << E;
    }
}


void Server::addChannel(channel* new_channel) {
    channels_lst.push_back(new_channel);
}

void Server::addClient(client* newClient)
{
    client_lst.push_back(newClient);
}

void Server::removeClient(client* existingClient)
{
    // std::remove pour déplacer les éléments à supprimer à la fin du vecteur
    std::vector<client*>::iterator it = std::remove(client_lst.begin(), client_lst.end(), existingClient);
    // erase pour supprimer les éléments déplacés
    client_lst.erase(it, client_lst.end());
}

client* Server::getNewClient() const
{
    return newClient;
}

int Server::getSizeClientList() const
{
    return (client_lst.size());
}

client* Server::getClientList(int x) const
{
    return (client_lst[x]);
}

void Server::setNewClient(client* client)
{
    newClient = client;
}

std::vector<channel *>& Server::getChannelsList()
{
    return channels_lst;
}

std::vector<client*>& Server::getClientList()
{
    return client_lst;
}

void Server::printChannelsAndClients() const
{
    for (size_t i = 0; i < channels_lst.size(); ++i) {
        std::cout << "Channel: " << channels_lst[i]->getName() << std::endl;
        std::vector<client*> clients = channels_lst[i]->getClients();
        for (size_t j = 0; j < clients.size(); ++j) {
            std::cout << "  Client: " << clients[j]->getNickname() << std::endl;
        }
    }
}

int Server::getIterator()
{
    return (iterator);
}

void Server::setIterator(int i)
{
    iterator = i;
}

std::vector<struct pollfd>& Server::getPollFd()
{
    return (_poll_fds);
}

void Server::sendToClient(int client_fd, const std::string &message) 
{
    send(client_fd, message.c_str(), message.size(), 0);
}

std::string Server::getPassword()
{
    return _password;
}

std::string Server::getServerName(){
    return _server_name;
}

void Server::clearChannels() {
    for (size_t i = 0; i < channels_lst.size(); ++i) {
        delete channels_lst[i]; // Libérer chaque canal
    }
    channels_lst.clear(); // Vider le vecteur
    std::cout << "All channels have been cleared." << std::endl;
}
