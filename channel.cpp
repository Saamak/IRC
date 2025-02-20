#include "includes/channel.hpp"

channel::channel(std::string name): _name(name){
    P << "channel constructor" << E;
}

channel::~channel(){
    P << "destructor channel" << E;
}

std::string channel::getName() const {
    return _name;
}