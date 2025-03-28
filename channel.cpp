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

void    channel::setChannelFlag(std::string flag)
{
	P << flag.size() << E;
	if(flag.size() > 2)
	{
		P << B_R <<"erreur flag DE MERDE" << E;
		return ;
	}
	if (flag[0] != '-' && flag[0] != '+')
	{
		P << B_R <<"+ - ERROR" << E;
		return ;
	}
	switch (flag[1]) 
	{
		case 'i':
		isInvitOnly = !isInvitOnly;
		P << B_G << "InvitObly set to : " << getOpTopic() << E;
		break;
		case 't':
		opTopic = !opTopic;
		P << B_G << "op topic set to : " << getOpTopic() << E;
		break;
		case 'k':
		isPasswd = !isPasswd;
		P << B_G << "KeyMode set to : " << getOpTopic() << E;
		break;
		case 'o':
		P<< "flag" << BOLD << " o " <<RESET << "non pris en charge pour le moment" << E;
		break;
		case 'l':
		P << "flag" << BOLD << " l " <<RESET << "non pris en charge pour le moment" << E;
		break;
		default:
		P << B_R << "Flag inconnu" << E;
		break;
	}
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

std::vector<client*> channel::getClients() const {return (client_lst);}

std::string channel::getTopic() const {return (_topic);}

void channel::setTopic(const std::string topic_name) {_topic = topic_name;}

bool channel::getIsInvitOnly() const {return (isInvitOnly);}

void channel::setIsInvitOnly(bool value) { isInvitOnly = value; }

bool channel::getOpTopic() const {return (opTopic);}

void channel::setOpTopic(bool value) { opTopic = value; }

bool channel::getIsPasswd() const {return (isPasswd);}

void channel::setIsPasswd(bool value) { isPasswd = value; }

std::string channel::getName() const {return (_name);}
