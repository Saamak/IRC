#include "includes/channel.hpp"


void channel::addClient(client* newClient)
{
	std::cout << "[DEBUG] Adding client " << newClient->getNickname() << " to channel " << _name << std::endl;
	client_lst.push_back(newClient);
}

bool    channel::IsInChannel(const std::string &nickname)
{
	for (std::vector<client*>::const_iterator it = client_lst.begin(); it != client_lst.end(); ++it) 
	{
		if ((*it)->getNickname() == nickname)
		return true;
	}
	return false;
}

bool    channel::IsOperator(const std::string &name)
{
	for (std::vector<client*>::const_iterator it = operator_lst.begin(); it != operator_lst.end(); ++it) 
	{
		if ((*it)->getNickname() == name)
		return true;
	}
	return false;
}

bool    channel::ClientExist(const std::string &name)
{
	for (size_t x = 0; x < client_lst.size(); x++)
	{
		if (client_lst[x]->getNickname() == name)
		return true;
	}
	return false;
}


channel::channel(std::string name): _name(name)
{
	P << "channel constructor" << E;
	isInvitOnly = false;
	isPasswd = false;
	opTopic = false;
	maxUser = std::numeric_limits<size_t>::max();
}

channel::~channel() { P << "destructor channel" << E; }

void channel::addOperator(client * newOperator) { operator_lst.push_back(newOperator); }

void channel::removeOperator(client *noOperator) {
    for (std::vector<client*>::iterator it = operator_lst.begin(); it != operator_lst.end(); ++it) {
        if (*it == noOperator) {
            operator_lst.erase(it); // Supprime l'opérateur de la liste
            std::cout << "Operator removed: " << noOperator->getNickname() << std::endl;
            return;
        }
    }
    std::cout << "Operator not found: " << noOperator->getNickname() << std::endl;
}

std::vector<client*> channel::getClients() const {return (client_lst);}

std::string channel::getTopic() const {return (_topic);}

void channel::setTopic(const std::string topic_name) {_topic = topic_name;}

bool channel::getIsInvitOnly() const {return (isInvitOnly);}

void channel::setIsInvitOnly(bool value) { isInvitOnly = value; }

bool channel::getOpTopic() const {return (opTopic);}

void channel::setOpTopic(bool value) { opTopic = value; }

bool channel::getIsPasswd() const {return (isPasswd);}

void channel::setIsPasswd(bool value) { isPasswd = value; }

void channel::setKey(std::string keyValue){key = keyValue;}

std::string channel::getKey(){return key;}

std::string channel::getName() const {return (_name);}
