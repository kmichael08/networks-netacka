
#ifndef NETWORKS_NETACKA_GAME_H
#define NETWORKS_NETACKA_GAME_H

#include <cstdint>
#include <vector>
#include "event.h"

using namespace std;

class Board {
    uint32_t width, height;
    uint8_t** board; /* 0 - empty, 1 - body */
public:
    Board(uint32_t width, uint32_t height);
    bool is_occupied(uint32_t x, uint32_t y);
    uint32_t get_width();
    uint32_t get_height();
    bool inside_board(double x, double y);
    /* Put a snake head on a given pixel */
    void take(uint32_t x, uint32_t y);
};

class Player {
    uint32_t session_id;
    char* name;
    double headx, heady;
    double direction;
    bool alive;
    static const uint32_t MAX_DIRECTION = 360;
    uint32_t last_activity_time;
    static const uint32_t TWO_SECS_IN_MICROSECS = 2000000;
public:
    Player(uint32_t session_id, char* name);
    void set_parameters(double headx, double heady, double direction);
    /* Player should have the same session_id */
    uint32_t get_session_id();
    double get_headx();
    double get_heady();
    double get_direction();
    char* get_name();
    bool is_alive();
    void reborn(); /* alive again - new game starts */
    void kill();
    void turn(int8_t turn_direction, uint32_t turning_speed);
    void move(); /* Make a move in the current direction with a distance 1 */
    bool not_responding(); /* Client not responding for two seconds */
};


class Game {
    uint32_t game_id;
    Board* board;
    vector<Event*> all_events; /* all events from the start of the game */
    vector<Player*> players; /* players sorted by their names */
    uint32_t turn_time;
    uint32_t turning_speed;
public:
    Game(uint32_t game_id, uint32_t width, uint32_t height, vector<Player*>& players,
         uint32_t turn_time, uint32_t turning_speed);

    void move_snake(int8_t turn_direction, Player* player, uint8_t player_num);

    /* Add events of different types respectively */
    void add_new_game(uint32_t width, uint32_t height, vector<char*>& players_names);
    void add_pixel(uint8_t player_number, uint32_t x, uint32_t y);
    void add_player_eliminated(uint8_t player_number);
    void add_game_over();
    Board* get_board();
    /* true - less than 2 players left, otherwise false */
    bool end_game();
};



#endif //NETWORKS_NETACKA_GAME_H
