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
    /* parse events from raw_data, as long as they're correct */
    static vector<Event*>& parse_events(char* event_data, size_t len);
    /* Take the event from the raw_data, return nullptr if there is any error */
    /* parse event_data and return event with given parameters, if error ocurrs return nullptr */
    static Event* parse_single_event_data(
            uint32_t events_length, uint32_t event_no, uint8_t event_type, uint32_t crc32,
            char* event_data, size_t event_data_length
    );
};


class NewGame : public Event {
    uint32_t maxx, maxy; /* width and height in pixels */
    vector<char*> players_names_list;
public:
    NewGame(uint32_t event_no, uint32_t maxx, uint32_t maxy, vector<char*>& players_names_list);
    char* raw_data();
    uint32_t raw_data_len();
    static Event* parse_single_event_data(uint32_t event_no, char* event_data, size_t event_data_length
    );

};

class Pixel : public Event {
    uint8_t player_number;
    uint32_t x, y; /* position */
public:
    Pixel(uint32_t event_no, uint8_t player_number, uint32_t x, uint32_t y);
    char* raw_data();
    uint32_t raw_data_len();
    static Event* parse_single_event_data(uint32_t event_no, char* event_data, size_t event_data_length);

};

class PlayerEliminated : public Event {
    uint8_t player_number;
public:
    PlayerEliminated(uint32_t event_no, uint8_t player_number);
    char* raw_data();
    uint32_t raw_data_len();
    static Event* parse_single_event_data(uint32_t event_no, char* event_data, size_t event_data_length
    );

};

class GameOver : public Event {
public:
    GameOver(uint32_t event_no);
    char* raw_data();
    uint32_t raw_data_len();
    static Event* parse_single_event_data(uint32_t event_no, size_t event_data_length);

};



#endif //NETWORKS_NETACKA_EVENT_H
