#include "server.hpp"
#include "colors.h"


class client{
    private :
        std::string _nickname;

    public :
        client();
        ~client();
        std::string getNickname();
        void setNickname(std::string nickName);
};