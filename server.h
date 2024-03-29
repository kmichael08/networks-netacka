#ifndef NETWORKS_NETACKA_SERVER_H
#define NETWORKS_NETACKA_SERVER_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include <poll.h>
#include "game.h"
#include "datagramClientToServer.h"
#include <netinet/in.h>

using VPIT = vector<Player*>::iterator;

using namespace std;

class Server {
private:
    uint64_t seed;
    uint64_t current_random_number;
    uint32_t width = 800, height = 600, port = 12345, speed = 50, turning_speed = 6;
    static const uint64_t RANDOMIZATION_FACTOR = 279470273;
    static const uint64_t RANDOMIZATION_MODULO = 4294967291;
    static const int MAX_CLIENTS = 42;
    static const int TIMEOUT_MILLISECS = 2;
    vector<Player*> players; /* Active players */
    vector<Player*> spectators; /* Players with empty names, just watching */

    uint32_t next_random_number();

    void parse_arguments(int argc, char* argv[]);

    uint64_t get_turn_time(); /* In microseconds */
    pollfd* sock;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t snda_len, rcva_len;
    const int MAX_CLIENT_DATAGRAM_SIZE = 80;
    Game* current_game;
    uint32_t first_event_not_sent_to_all = 0;
    bool active_game = 0;
    uint64_t turn_time;
public:
    Server(int argc, char* argv[]);

    void print_arguments();

    /* Init the new game. */
    void new_game();

    /* Init starting positions and directions of the players */
    void init_players(vector<Player*> &current_players, Game* game);

    /**
    * @return if true then server listens datagrams, otherwise it broadcasts datagrams to clients.
    */
    bool udp_listen();

    void receive_udp();

    /**
    * Send udp datagram.
    */
    void send_udp(Player* player, char* datagram, size_t len);

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

    static void reset_player(Player* player, vector<Player*>& players_list);

    /* reset player - replace the old one with a new one with the same parameters */
    void reset_player(Player *player, bool is_spectator);

    /* check if the name already exists, if not empty */
    bool name_exist(char* name);

    void send_to_all(); /* send all recent events to everyone listening */

    bool is_game_active() const;

    Game* get_current_game();

    void finish_game();

    bool all_players_ready() const;

    void disconnect_not_responding_users();

    static void disconnect_not_responding_users(vector<Player*>& players_list);

    static VPIT find_player(Player* player, vector<Player*>& players_list);

};



#endif //NETWORKS_NETACKA_SERVER_H
