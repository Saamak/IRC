#include "includes/server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "includes/command.hpp"
#include <algorithm> 

void    Server::setBoolExit(bool tmp)
{
    _exit = tmp;
}

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

bool Server::init()
{
    _exit = false;
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
    std::cout << G"Client connected\n" << RESET << std::endl;

    struct pollfd client_pollfd;
    client_pollfd.fd = client_socket;
    client_pollfd.events = POLLIN;
    client_pollfd.revents = 0;
    _poll_fds.push_back(client_pollfd);

    std::string client_data;
    int i = _poll_fds.size() - 1;
    char buffer[1024];
    int count = 0;
    while (count < 4)
    {
        int bytes_read = read(_poll_fds[i].fd, buffer, sizeof(buffer));
        buffer[bytes_read] = '\0';
        int x = 0;
        while (buffer[x])
        {
            if (buffer[x] == '\n')
                count++;
            x++;
        }
        std::string buff(buffer);
        client_data = client_data + buff;
    }
    std::vector<std::string> tokens = split(client_data, '\n');
    std::string test("\n");
    tokens.push_back(test);
    for (int y = 0; y < 4; y++)
    {
       std::cout << B_R << tokens[y] << RESET << E;
       integrity(tokens[y]);
    }
}

int Server::HandleCommunication(int i)
{
    // Handle client communication
    char buffer[1024];
    int bytes_read = read(_poll_fds[i].fd, buffer, sizeof(buffer));
    if (bytes_read <= 0) 
    {
        // Client disconnected
        if (bytes_read == 0) {
            std::cout << "Client disconnected" << std::endl;
        } else {
            P << "serv : " << _server_fd << E;
            P << _poll_fds[i].fd << E;
            P << bytes_read << E;
            std::cerr << "Error: read failed" << std::endl;
        }
        close(_poll_fds[i].fd);
        _poll_fds.erase(_poll_fds.begin() + i);
        return (--i); // Adjust index after erasing
    }
    else
    {
        // Process client data
        buffer[bytes_read] = '\0';
        std::cout << B_Y "Received: " << RESET << buffer << std::endl;
        // Echo the data back to the client
        std::string client_data(buffer);
        integrity(client_data);
        write(_poll_fds[i].fd, buffer, bytes_read);
    }
    return (i);
}

void Server::start()
{
    if (listen(_server_fd, 5) < 0)
    {
        std::cerr << "Error: listen failed" << std::endl;
        return;
    }

    // std::cout << "Server listening on port " << B_Y << _port << RESET << std::endl;

    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    _poll_fds.push_back(server_pollfd);

    while (true)
    {
        if (_exit == true)
            return ;
        int poll_count = poll(_poll_fds.data(), _poll_fds.size(), -1);
        if (poll_count < 0)
        {
            std::cerr << "Error: poll failed" << std::endl;
            return;
        }
        for (size_t i = 0; i < _poll_fds.size(); ++i)
        {
            iterator = i; //PAS SURE car size_t to int et autre prblm;
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
        for(int x = 0; x < getIterator(); x++)
        {
            P << client_lst[x]->getNickname() << E;
            P << client_lst[x]->getUsername() << E;
        }
    }
}

void Server::stop() {
    if (_server_fd != -1) {
        close(_server_fd);
        _server_fd = -1;
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

void Server::sendToClient(int client_fd, const std::string &message) 
{
    send(client_fd, message.c_str(), message.size(), 0);
}