#include "includes/server.hpp"
#include "includes/command.hpp"


bool exit_b = false;

bool Server::clientExists(const std::string &nickname) const
{
	for (size_t i = 0; i < client_lst.size(); i++)
	{
		if (client_lst[i]->getNickname() == nickname)
			return true;
	}
	return false;
}

client* Server::getClientByNickname(const std::string &nickname) const
{
	for (size_t i = 0; i < client_lst.size(); i++)
	{
		if (client_lst[i]->getNickname() == nickname)
			return client_lst[i];
	}
	return NULL;
}

int Server::getClientFd(const std::string &nickname) const
{
	for (size_t i = 0; i < client_lst.size(); i++)
	{
		if (client_lst[i]->getNickname() == nickname)
			return _poll_fds[i + 1].fd;
	}
	return -1;
}


void Server::integrity(std::string client_data) {
	command cmdd(*this);
	cmdd.exec(client_data);
}

void Server::myExit()
{
    clearChannels();
    for (size_t i = 0; i < client_lst.size(); ++i) {
        if (client_lst[i] != NULL) {
            delete client_lst[i];
            client_lst[i] = NULL;
        }
    }
    client_lst.clear();
    for (size_t x = 0; x < _poll_fds.size(); x++)
        close(_poll_fds[x].fd);
    _poll_fds.clear();

    std::cout << "Server shut down successfully. All memory freed." << std::endl;
}

bool Server::init(char *pass)
{
	_server_name = "xxX.PPPBestIrc.Xxx";
	_password = pass;
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
	{
		std::cerr << "Error: socket creation failed" << std::endl;
		return false;
	}

	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
	{
		std::cerr << "Error: setsockopt failed" << std::endl;
		close(_server_fd);
		return false;
	}
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1) 
	{
	std::cerr << "Error: fcntl set failed" << std::endl;
	close(_server_fd);
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
	std::cout << G"Client connected SAS\n" << RESET << std::endl;
	struct pollfd client_pollfd;
	client_pollfd.fd = client_socket;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;
	_poll_fds.push_back(client_pollfd);
	
	newClient = new client();
	addClient(newClient);
	
}


void handleSignal(int signal)
{
	if(signal == SIGINT)
	{
		std::cout << BOLD << "SIGINT Received" << E;
		exit_b = true;
		return ;
	}
	if (signal == SIGQUIT)
	{
		std::cout << BOLD << "SIGQUIT Received" << E;
		exit_b = true;
		return ;
	}
}

void set_signal()
{
	signal(SIGINT, handleSignal);
	signal(SIGQUIT, handleSignal);
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
        set_signal();
        if(exit_b == true)
		{
            myExit();
			return ;
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
                    clientConnected();
                else 
                    i = HandleCommunication(i);
            }
        }
        printServerStatus();
    }
}

int Server::HandleCommunication(int i)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	
	int bytes_read = read(_poll_fds[i].fd, buffer, sizeof(buffer) - 1);
	
	if (bytes_read <= 0) 
	{
		std::cout << "Client disconnected" << std::endl;
		integrity("QUIT");
		return (i);
	}
	
	buffer[bytes_read] = '\0';
	std::string client_test(buffer);
	
	if (iterator > 0 && iterator <= client_lst.size()) 
	{
		std::string buffer_client_tmp = client_lst[iterator - 1]->getBufferClient();
		
		if (!buffer_client_tmp.empty())
		{
			try 
			{
				client_test = buffer_client_tmp + client_test;
			} 
			catch (const std::exception &e) 
			{
				std::cerr << "String error: " << e.what() << std::endl;
				client_lst[iterator - 1]->emptyBufferClient();
				return (i);
			}
		}
		
		if (client_test.find('\n') == std::string::npos)
		{
			if (client_test.length() > 4096) 
			{
				client_lst[iterator - 1]->emptyBufferClient();
				std::string message = "Error: Message too large\r\n";
				send(_poll_fds[i].fd, message.c_str(), message.size(), 0);
			} else 
				client_lst[iterator - 1]->setBufferClient(client_test);
			return (i);
		}
		
		client_lst[iterator - 1]->emptyBufferClient();
		std::vector<std::string> splitted = split(client_test, '\n');
		for(size_t x = 0; x < splitted.size(); x++)
			integrity(splitted[x]);
	}
	else
	{
		std::cerr << "Invalid iterator value: " << iterator << std::endl;
		return (i);
	}
	return (i);
}

void Server::addChannel(channel* new_channel) {
	channels_lst.push_back(new_channel);
}

void Server::addClient(client* newClient)
{
	client_lst.push_back(newClient);
}

void Server::clearChannels()
{
    // Parcourir et libérer chaque canal
    for (size_t i = 0; i < channels_lst.size(); ++i) {
        if (channels_lst[i] != NULL) {
            channels_lst[i]->getClients().clear();
            channels_lst[i]->getOperators().clear();
            channels_lst[i]->getInviteList().clear();
            delete channels_lst[i];
            channels_lst[i] = NULL;
        }
    }
    channels_lst.clear();
}

Server::Server(int port) : _port(port), _server_fd(-1), newClient(NULL) {
	std::memset(&_server_addr, 0, sizeof(_server_addr));
}

Server::~Server() {
	if (_server_fd != -1)
		close(_server_fd);
}

void Server::printServerStatus() 
{
    time_t now = time(0);
    char* dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0';

    // Entête
    std::cout << "\n" << B_G << "┌─────────────────────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << B_G << "│                    IRC SERVER STATUS                     │" << RESET << std::endl;
    std::cout << B_G << "│                 " << dt << "                 │" << RESET << std::endl;
    std::cout << B_G << "└─────────────────────────────────────────────────────────┘" << RESET << std::endl;
    
    // Informations générales
    std::cout << Y << "• Connected clients: " << BOLD << client_lst.size() << RESET 
              << Y << " | Active file descriptors: " << BOLD << _poll_fds.size() << RESET 
              << Y << " | Channels: " << BOLD << channels_lst.size() << RESET << std::endl;
    
    // Séparateur
    std::cout << B_G << "┌─────────────────────────────────────────────────────────┐" << RESET << std::endl;
    
    // Détails des canaux
    if (channels_lst.empty()) {
        std::cout << Y << "│ " << RESET << "No active channels" << std::endl;
    } else {
        for (size_t i = 0; i < channels_lst.size(); i++) {
            std::string modes = channels_lst[i]->getModes();
            size_t memberCount = channels_lst[i]->getNumberClient();
            size_t limit = channels_lst[i]->getLimit();
            
            std::string limitStr;
            if (limit == std::numeric_limits<size_t>::max()) {
                limitStr = "∞";
            } else {
                std::ostringstream oss;
                oss << limit;
                limitStr = oss.str();
            }
            
            // En-tête du canal
            std::cout << B_M << "│ CHANNEL: " << BOLD << channels_lst[i]->getName() 
                      << RESET << B_M << " | MODE: " << modes 
                      << " | USERS: " << memberCount << "/" << limitStr
                      << RESET << std::endl;
            
            if (!channels_lst[i]->getTopic().empty()) {
                std::cout << G << "│ TOPIC: " << RESET << channels_lst[i]->getTopic() << std::endl;
            }
            
            // Liste des clients
            std::cout << Y << "│ MEMBERS:" << RESET << std::endl;
            const std::vector<client*>& clients = channels_lst[i]->getClients();
            for (size_t j = 0; j < clients.size(); j++) {
                std::string nickname = clients[j]->getNickname();
                std::string role = channels_lst[i]->IsOperator(nickname) ? "@" : " ";
                std::cout << "│   " << role << " " << nickname;
                
                // Afficher aussi le username si disponible
                if (!clients[j]->getUsername().empty() && clients[j]->getUsername() != "unknown") {
                    std::cout << " (" << clients[j]->getUsername() << ")";
                }
                
                std::cout << std::endl;
            }
            
            // Liste des invitations
            const std::vector<std::string>& invites = channels_lst[i]->getInviteList();
            if (!invites.empty()) {
                std::cout << Y << "│ INVITES:" << RESET << std::endl;
                for (size_t k = 0; k < invites.size(); k++) {
                    std::cout << "│   " << invites[k] << std::endl;
                }
            }
            
            // Séparateur entre les canaux
            if (i < channels_lst.size() - 1) {
                std::cout << B_G << "├─────────────────────────────────────────────────────────┤" << RESET << std::endl;
            }
        }
    }
    
    // Pied de page
    std::cout << B_G << "└─────────────────────────────────────────────────────────┘\n" << RESET << std::endl;
}

client* Server::getNewClient() const {return (newClient);}

int Server::getSizeClientList() const {return (client_lst.size());}

client* Server::getClientList(int x) const {return (client_lst[x]);}

void Server::setNewClient(client* client) { newClient = client; }

std::vector<channel *>& Server::getChannelsList() {return (channels_lst);}

std::vector<client*>& Server::getClientList(){return (client_lst);}

int Server::getIterator(){return (iterator);}

void Server::setIterator(int i) { iterator = i; }

std::vector<struct pollfd>& Server::getPollFd() {return (_poll_fds);}

std::string Server::getPassword() {return (_password);}

std::string Server::getServerName() {return (_server_name);}
