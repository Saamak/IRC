#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "server.hpp"
#include "colors.h"


class channel{
    private :
        std::string _name;
    public :
        channel(std::string name);
        ~channel();
        std::string getName() const;
        //vecteur  client membre du channel
};

#endif