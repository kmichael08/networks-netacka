
#ifndef NETWORKS_NETACKA_GAME_H
#define NETWORKS_NETACKA_GAME_H

#include <cstdint>
#include <vector>
#include "event.h"

using namespace std;

class Board {
    uint32_t width, height;
    uint32_t** board; /* 0 - empty, 1 - body, 2 - head */
public:
    Board(uint32_t width, uint32_t height);
    bool is_occupied(uint32_t x, uint32_t y);
};


class Game {
    uint32_t game_id;
    Board* board;
    vector<Event*> all_events;
    // players also
public:
    Game(uint32_t game_id);
};



#endif //NETWORKS_NETACKA_GAME_H
