//
// Created by michal on 03.06.17.
//

#ifndef NETWORKS_NETACKA_EVENT_H
#define NETWORKS_NETACKA_EVENT_H

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

class EventData {
public:
    virtual uint8_t event_no() = 0;
};

class NewGame : EventData {
    uint32_t maxx, maxy; /* width and height in pixels */
    vector<string> players_list;
public:
    NewGame(uint32_t maxx, uint32_t maxy, vector<string>& players_list);

    uint8_t event_no();
};

class Pixel : EventData {
    uint8_t player_number;
    uint32_t x, y;
public:
    Pixel(uint8_t player_number, uint32_t x, uint32_t y);

    uint8_t event_no();
};

class PlayerEliminated : EventData {
    uint8_t player_number;
public:
    PlayerEliminated(uint8_t player_number);

    uint8_t event_no();
};

class GameOver : EventData {
public:
    GameOver();

    uint8_t event_no();
};

class Event {
    uint32_t  len; /* total length of fields event_* */
    uint32_t event_no;
    uint8_t event_type; /* {0, 1, 2, 3} */
    uint32_t crc32; /* checksum */
    EventData* event_data;
};



#endif //NETWORKS_NETACKA_EVENT_H
