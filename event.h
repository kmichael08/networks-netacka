//
// Created by michal on 03.06.17.
//

#ifndef NETWORKS_NETACKA_EVENT_H
#define NETWORKS_NETACKA_EVENT_H

#include <cstdint>
#include <string>
#include <vector>

using namespace std;


class Event {
protected:
    uint32_t  len; /* total length of fields event_* */
    uint32_t event_no;
    uint8_t event_type = 0; /* {0, 1, 2, 3} */
    uint32_t crc32; /* checksum */
public:
    Event(uint32_t event_no);
};


class NewGame : public Event {
    uint32_t maxx, maxy; /* width and height in pixels */
    vector<string> players_names_list;
public:
    NewGame(uint32_t event_no, uint32_t maxx, uint32_t maxy, vector<string>& players_names_list);
};

class Pixel : public Event {
    uint8_t player_number;
    uint32_t x, y;
public:
    Pixel(uint32_t event_no, uint8_t player_number, uint32_t x, uint32_t y);
};

class PlayerEliminated : public Event {
    uint8_t player_number;
public:
    PlayerEliminated(uint32_t event_no, uint8_t player_number);
};

class GameOver : public Event {
public:
    GameOver(uint32_t event_no);
};



#endif //NETWORKS_NETACKA_EVENT_H
