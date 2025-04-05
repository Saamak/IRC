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
	command cmdd(*this); // Passe une référence au serveur
	cmdd.exec(client_data);
}

void Server::myExit()
{
    // 1. D'abord nettoyer tous les canaux
    clearChannels();

    // 2. Ensuite libérer tous les clients
    for (size_t i = 0; i < client_lst.size(); ++i) {
        if (client_lst[i] != NULL) {
            delete client_lst[i];
            client_lst[i] = NULL;
        }
    }
    client_lst.clear();

    // 3. Fermer tous les descripteurs de fichiers
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
	
	int flags = fcntl(_server_fd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error: fcntl get failed" << std::endl;
		close(_server_fd);
		return false;
	}
	if (fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) == -1) 
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
		std::vector<channel*> channels = getChannelsList();
		P << B_M "Number of Channels : " << channels.size() << RESET << E;
		for (size_t x = 0; x < channels.size(); x++)
		{
			std::vector<client*> clients = channels[x]->getClients();
			std::vector<client*> operators = channels[x]->getOperators();
			std::vector<std::string> invitations = channels[x]->getInviteList();
			P << B_M << channels[x]->getName() << RESET << E;
			for (size_t y = 0; y < clients.size(); y++)
			{
				if (y == 0)
					P << B_M "Clients : " RESET << E;
				P << B_M "     - " << clients[y]->getNickname() << RESET << E; 
			}
			for (size_t z = 0; z < operators.size(); z++)
			{
				if (z == 0)
					P << B_M "Operators : " RESET << E;
				P << B_M "     - " << operators[z]->getNickname() <<  RESET << E;
			}
			for (size_t a = 0; a < invitations.size(); a++)
			{
				if (a == 0)
					P << B_M "Invitations : " RESET << E;
				P << B_M "     - " << invitations[a] << RESET << E;;
			}
		}
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
		{
			P << B_Y << splitted[x] << RESET << E;
			integrity(splitted[x]);
		}
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

void Server::removeClient(client* existingClient)
{
	// std::remove pour déplacer les éléments à supprimer à la fin du vecteur
	std::vector<client*>::iterator it = std::remove(client_lst.begin(), client_lst.end(), existingClient);
	// erase pour supprimer les éléments déplacés
	client_lst.erase(it, client_lst.end());
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

void Server::removeChannel(std::vector<channel*>::iterator i)
{
	channels_lst.erase(i);
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

client* Server::getNewClient() const {return (newClient);}

int Server::getSizeClientList() const {return (client_lst.size());}

client* Server::getClientList(int x) const {return (client_lst[x]);}

void Server::setNewClient(client* client) { newClient = client; }

std::vector<channel *>& Server::getChannelsList() {return (channels_lst);}

std::vector<client*>& Server::getClientList(){return (client_lst);}

int Server::getIterator(){return (iterator);}

void Server::setIterator(int i) { iterator = i; }

std::vector<struct pollfd>& Server::getPollFd() {return (_poll_fds);}

void Server::sendToClient(int client_fd, const std::string &message) { send(client_fd, message.c_str(), message.size(), 0); }

std::string Server::getPassword() {return (_password);}

std::string Server::getServerName() {return (_server_name);}
