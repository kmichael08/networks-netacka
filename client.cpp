#include <cstdint>
#include <cstring>
#include "datagramServerToClient.h"
#include "datagramClientToServer.h"
#include "err.h"

uint32_t game_server_port = 12345, ui_server_port = 12346;
char* ui_server_name;
char* game_server_name;
char* player_name;

void print_arguments() {
    cout << player_name << " " << game_server_name << ":" << game_server_port << " " << ui_server_name << ":" << ui_server_port << endl;
}

/**
 * @param arg string from arguments
 * @param name name to be modified
 * @param port port to be modified
 */
void parse_names(char* arg, char*& name, uint32_t& port) {
    char* separator = strchr(arg, ':');
    if (separator == nullptr) {
        name = arg;
    }
    else {
        size_t name_len = separator - arg;
        if (name_len == 0)
            syserr("Name should not be empty");


        name = new char[name_len + 1];
        strncpy(name, arg, name_len);
        name[name_len] = '\0';

        port = get_number(separator + 1);
    }

}

void parse_arguments(int argc, char* argv[]) {
    if (argc < 3 || argc > 4)
        syserr("Bad arguments number - usage ./siktacka-client player_name game_server_host[:port] [ui_server_host[:port]]");
    player_name = argv[1];

    if (!DatagramClientToServer::valid_player_name(player_name))
        syserr("Wrong player name");

    parse_names(argv[2], game_server_name, game_server_port);

    if (argc == 4) {
        parse_names(argv[3], ui_server_name, ui_server_port);
    }
    else {
        ui_server_name = new char[10];
        strncpy(ui_server_name, "localhost", 9);
        ui_server_name[10] = '\0';
    }


}

int main(int argc, char* argv[]) {
    parse_arguments(argc, argv);
    print_arguments();
}