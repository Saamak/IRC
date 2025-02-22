#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "server.hpp"
#include "colors.h"


class channel{
    private :
        std::string _name;
        bool isInvitOnly;
        bool opTopic;
        std::string key;
        size_t maxUser;
        std::vector<client*> client_lst;
        std::vector<client*> operator_lst
    public :
        channel(std::string name);
        ~channel();
        void    addClient(client* newClient)
        bool    IsInChannel(const std::string &name);


        //SETTER GETTER
        std::string getName() const;

};

#endif