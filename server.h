#ifndef NETWORKS_NETACKA_SERVER_H
#define NETWORKS_NETACKA_SERVER_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include <poll.h>
#include "game.h"
#include "datagramClientToServer.h"
#include <netinet/in.h>

using namespace std;

class Server {
private:
    uint64_t seed;
    uint64_t current_random_number;
    uint32_t width = 800, height = 600, port = 12345, speed = 50, turning_speed = 6;
    static const uint64_t RANDOMIZATION_FACTOR = 279470273;
    static const uint64_t RANDOMIZATION_MODULO = 4294967291;
    static const int MAX_CLIENTS = 42;
    static const int TIMEOUT_MILLISECS = 20;
    vector<Player*> players; /* Active players */
    vector<Player*> spectators; /* Players with empty names, just watching */

    uint32_t next_random_number();

    void parse_arguments(int argc, char* argv[]);

    uint32_t turn_time();
    pollfd* sock;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t snda_len, rcva_len;
    const int MAX_CLIENT_DATAGRAM_SIZE = 80;
    Game* current_game;

public:
    Server(int argc, char* argv[]);

    void print_arguments();

    /* Init the new game, with a given parameters */
    Game* new_game(uint32_t width, uint32_t height, vector<Player*>& players);

    /* Init starting positions and directions of the players */
    void init_players(vector<Player*> &current_players, Game* game);

    /**
    * @return if true then server listens datagrams, otherwise it broadcasts datagrams to clients.
    */
    bool listen();

    void receive_udp();

    /**
    * Send udp datagram.
    */
    void send_udp(Player* player, char* datagram);

    /** Send all events staring from first_event to the player via udp,
     *  possibly splitted as to fit in the max udp size
     */
    void send_events(uint32_t first_event, Player* player);

    /* Return player with a given client_address, nullptr if there is not */
    Player* get_player(sockaddr_in* client_address);

    /* get the player of a given address in the given vector */
    static Player* get_player(sockaddr_in* client_address, vector<Player*>& players_list);

    /* Current number of active players and spectators */
    int current_players_number();

    /* Add new_player, it is ensured it can be added */
    Player* add_new_player(DatagramClientToServer *datagram, sockaddr_in *client_address);
};



#endif //NETWORKS_NETACKA_SERVER_H
