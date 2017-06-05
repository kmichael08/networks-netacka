#ifndef NETWORKS_NETACKA_SERVER_H
#define NETWORKS_NETACKA_SERVER_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include "game.h"

using namespace std;

class Server {
private:
    uint64_t seed;
    uint64_t current_random_number;
    uint32_t width = 800, height = 600, port = 12345, speed = 50, turning_speed = 6;
    static const uint64_t RANDOMIZATION_FACTOR = 279470273;
    static const uint64_t RANDOMIZATION_MODULO = 4294967291;
    static const int MAX_CLIENTS = 42;
    vector<Player*> players; /* Active players */
    vector<Player*> spectators; /* Players with empty names, just watching */

    uint32_t next_random_number();

    void parse_arguments(int argc, char* argv[]);

    uint32_t turn_time();

public:
    Server(int argc, char* argv[]);

    void print_arguments();

    /* Init the new game, with a given parameters */
    Game* new_game(uint32_t width, uint32_t height, vector<Player*>& players);

    /* Init starting positions and directions of the players */
    void init_players(vector<Player*> &current_players, Game* game);
};



#endif //NETWORKS_NETACKA_SERVER_H
