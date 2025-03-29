#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "server.hpp"
#include "client.hpp"
#include <limits>
#include "colors.h"


class channel{
    private :
        std::string _name;
        std::string _topic;
        bool isInvitOnly;
        bool opTopic;
        bool isPasswd;
        std::string key;
        size_t maxUser;
        std::vector<client*> client_lst;
        std::vector<client*> operator_lst;

    public :
        channel(std::string name);
        ~channel();

        //UTILS
        void    addClient(client* newClient);
        bool    IsInChannel(const std::string &name);
        bool    ClientExist(const std::string &name);
        void    removeUser(const std::string &nickname);
        std::vector<client*> getClients() const;

        //Operator
        bool    IsOperator(const std::string &name);
        void    addOperator(client * newOperator);
       void     removeOperator(client *noOperator);




        //SETTER GETTER
        std::string getName() const;
        std::string getTopic() const;
        void setTopic(const std::string topic_name);
        bool getIsInvitOnly() const;
        bool getOpTopic() const;
        bool getIsPasswd() const;
        void setIsInvitOnly(bool value);
        void setOpTopic(bool value);
        void setIsPasswd(bool value);
        void setKey(std::string keyValue);
        std::string getKey();

};

#endif