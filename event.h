#ifndef NETWORKS_NETACKA_EVENT_H
#define NETWORKS_NETACKA_EVENT_H

#include <cstdint>
#include <string>
#include <vector>
#include "utils.h"

using namespace std;

class Event {
protected:
    uint32_t event_no;
    uint8_t event_type = 0; /* {0, 1, 2, 3} */
public:
    Event(uint32_t event_no);
    char* event_raw_data(); /* raw_data to send via udp representing whole event with its parameters */
    uint32_t event_raw_data_len(); /* full raw_data length */
    virtual char* raw_data() = 0; /* event_data */
    virtual uint32_t raw_data_len() = 0; /* length of event_data */
    static vector<Event*>& parse_events(char* event_data, size_t len); /* parse events from raw_data */
};


class NewGame : public Event {
    uint32_t maxx, maxy; /* width and height in pixels */
    vector<char*> players_names_list;
public:
    NewGame(uint32_t event_no, uint32_t maxx, uint32_t maxy, vector<char*>& players_names_list);
    char* raw_data();
    uint32_t raw_data_len();
};

class Pixel : public Event {
    uint8_t player_number;
    uint32_t x, y; /* position */
public:
    Pixel(uint32_t event_no, uint8_t player_number, uint32_t x, uint32_t y);
    char* raw_data();
    uint32_t raw_data_len();
};

class PlayerEliminated : public Event {
    uint8_t player_number;
public:
    PlayerEliminated(uint32_t event_no, uint8_t player_number);
    char* raw_data();
    uint32_t raw_data_len();
};

class GameOver : public Event {
public:
    GameOver(uint32_t event_no);
    char* raw_data();
    uint32_t raw_data_len();
};



#endif //NETWORKS_NETACKA_EVENT_H
