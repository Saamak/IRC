#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <csignal>

# define R     "\x1B[31m"
# define G     "\x1B[32m"
# define Y     "\x1B[33m"
# define B_R   "\x1B[1;31m"
# define B_G   "\x1B[1;32m"
# define B_Y   "\x1B[1;33m"
# define B_M  "\x1B[1;35m"
# define BOLD  "\x1B[1m"
# define RESET "\x1B[0m"

#define OFFSET_FRANCE 2
#define OFFSET_PAKISTAN 5
#define OFFSET_USA -4
#define OFFSET_BRAZIL -3
#define OFFSET_UK 1
#define OFFSET_JAPAN 9
#define OFFSET_AUSTRALIA 10
#define OFFSET_RUSSIA 3
#define OFFSET_CHINA 8

void send_command(int sockfd, const char* command);
void BotTime(int sockfd, const std::string country, const std::string channel);
void BotDrawHommer(int sockfd, const std::string channel);

#endif