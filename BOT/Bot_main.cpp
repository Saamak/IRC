#include "includes.hpp"

bool g_error = false;

void send_command(int sockfd, const char* command)
{
    std::cout << B_M "Envoi: " B_Y << command << RESET << std::endl;
    if (send(sockfd, command, strlen(command), 0) < 0) {
        std::cerr << "Erreur lors de l'envoi des données" << std::endl;
        close(sockfd);
        exit(1);
    }
}

std::string receive_response(int sockfd) {
    char buffer[1024];
    int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        std::cerr << "Erreur ou déconnexion du serveur" << std::endl;
        close(sockfd);
        exit(1);
    }
    buffer[bytes_received] = '\0';
    std::cout << B_Y "Réponse du serveur: " B_M << buffer << RESET << std::endl;
    return std::string(buffer);
}

void handlesignal(int signal)
{
    if (signal == SIGINT || signal == SIGSEGV)
    {
        g_error = true;
    }
}

void set_signal()
{
    signal(SIGINT, handlesignal);
    signal(SIGSEGV, handlesignal);
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << B_R "Not enough or bad parameters: ./bot server_IP Server_PORT Server_Password" RESET << std::endl;
        return 0;
    }
    std::string server_ip_string(argv[1]);
    const char* server_ip = server_ip_string.c_str();
    int server_port = atoi(argv[2]);
    if (server_port < 1024 || server_port > 65535)
	{
		std::cerr << "Error: Port must be between 1024 and 65535" << std::endl;
		return 1;
	}
    std::string password = argv[3];
    std::string full_password = "PASS " + password + "\r\n";
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << B_R "Erreur lors de la création du socket" RESET << std::endl;
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << B_R "Adresse IP invalide" RESET << std::endl;
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << B_R "Erreur lors de la connexion au serveur" RESET << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << B_G "Connecté au serveur " B_Y << server_ip << B_G " sur le port " B_Y << server_port << RESET << std::endl;

    bool log = false;
    struct pollfd fds[1];
    fds[0].fd = sockfd;
    fds[0].events = POLLOUT;

    while (true)
    {
        set_signal();
        if (g_error == true)
        {
            close(sockfd);
            return (1);
        }
        int ret = poll(fds, 1, -1);
        if (ret < 0) {
            std::cerr << "Erreur lors de l'appel à poll()" << std::endl;
            break;
        }

        if (log == false)
        {
            send_command(sockfd, full_password.c_str());
            send_command(sockfd, "NICK BOT\r\n");
            send_command(sockfd, "USER BOT\r\n");
            log = true;
            fds[0].events = POLLIN;
        }
        if (fds[0].revents & POLLIN)
        {
            std::string receive = receive_response(sockfd);
            std::istringstream iss(receive);
            std::string tmp;
            std::string channel;
            iss >> tmp >> tmp;
            if (tmp == "INVITE")
            {
                iss >> channel >> channel;
                std::string join_message = "JOIN " + channel + "\r\n";
                const char *str = join_message.c_str();
                send_command(sockfd, str);
            }
            else if (tmp == "PRIVMSG")
            {
                std::string input;
                iss >> channel;
                iss >> input;
                if (input == ":BOT_COMMAND")
                {
                    std::string join_message = "PRIVMSG " + channel + " :BOT all commands: \"BOT_TIME {LIST | COUNTRY}\"\t\"BOT_DRAW\"\r\n";
                    const char *str = join_message.c_str();
                    send_command(sockfd, str); 
                }
                if (input == ":BOT_TIME")
                {
                    std::string country;
                    iss >> country;
                    BotTime(sockfd, country, channel);
                }
                if (input == ":BOT_DRAW")
                    BotDrawHommer(sockfd, channel);
            }
        }   
    }
    close(sockfd);

    return 0;
}