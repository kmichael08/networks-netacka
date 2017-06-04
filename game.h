
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
    void take(uint32_t x, uint32_t y);
};

class Player {
    uint32_t session_id;
    string name;
    double headx, heady;
    double direction;
    bool alive;
    static const uint32_t MAX_DIRECTION = 360;
public:
    Player(uint32_t session_id, string name);
    void set_parameters(double headx, double heady, double direction);
    uint32_t get_session_id();
    double get_headx();
    double get_heady();
    double get_direction();
    string get_name();
    bool is_alive();
    void reborn(); /* alive again - new game starts */
    void turn(int8_t turn_direction, uint32_t turning_speed);
    void move();
};


class Game {
    uint32_t game_id;
    Board* board;
    vector<Event*> all_events;
    vector<Player*> players; /* players sorted by their names */
    uint32_t turn_time;
    uint32_t turning_speed;
public:
    Game(uint32_t game_id, uint32_t width, uint32_t height, vector<Player*>& players,
         uint32_t turn_time, uint32_t turning_speed);

    void move_snake(int8_t turn_direction, Player* player);
};



#endif //NETWORKS_NETACKA_GAME_H
