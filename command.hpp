#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <map>
#include <string>

// Déclaration du type de la fonction
typedef void(*CommandFunction)(void);

class command {
public:
    std::map<std::string, CommandFunction> _cmds;
    void exec(const std::string &client_data); // Utiliser une référence constante
    command();
    static void kick();
    static void invite();
    static void topic();
    static void mode();
};

#endif // COMMAND_HPP