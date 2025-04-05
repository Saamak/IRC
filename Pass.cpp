
#include "includes/command.hpp"

void command::pass(const std::string &client_data) // Pass daniel
{
	std::istringstream iss(client_data);
	std::string command;
	std::string password;
	iss >> command;
	iss >> password;
	std::vector<client*>& Client_tmp = _server.getClientList();
	std::vector<struct pollfd>& pollfd_tmp = _server.getPollFd();
	int fd = pollfd_tmp[_server.getIterator()].fd;
	size_t iterator = _server.getIterator() - 1;

	if (password.empty()) {
		// Utilisation de sendIrcMessage pour ERR_NEEDMOREPARAMS
		sendIt(ERR_NEEDMOREPARAMS(Client_tmp[iterator]->getNickname(), command), fd);
		return;
	}

	if (Client_tmp[iterator]->getRegistered()) {
		// Utilisation de sendIrcMessage pour ERR_ALREADYREGISTRED
		sendIt(ERR_ALREADYREGISTRED(Client_tmp[iterator]->getNickname()), fd);
		return;
	}

	if (!Client_tmp[iterator]->getRegistered() &&
		(Client_tmp[iterator]->getNickCheck() || Client_tmp[iterator]->getUserCheck())) {
		sendIt(ERR_BADCOMMANDORDER(Client_tmp[iterator]->getNickname()), fd);
		exec("QUIT");
		return;
	}
	Client_tmp[iterator]->setPassCheck(true);
	Client_tmp[iterator]->setClientPassword(password);
}