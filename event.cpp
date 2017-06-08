#include "event.h"
#include <cstring>
#include <netinet/in.h>
#include "err.h"
#include <iostream>

using namespace std;

NewGame::NewGame(uint32_t event_no, uint32_t maxx, uint32_t maxy, vector<char*>& players_names_list) :
    Event(event_no, 0), maxx(maxx), maxy(maxy), players_names_list(players_names_list)
{}

Pixel::Pixel(uint32_t event_no, uint8_t player_number, uint32_t x, uint32_t y) :
    Event(event_no, 1), player_number(player_number), x(x), y(y)
{}

PlayerEliminated::PlayerEliminated(uint32_t event_no, uint8_t player_number) :
    Event(event_no, 2), player_number(player_number)
{}

GameOver::GameOver(uint32_t event_no):
    Event(event_no, 3)
{}

Event::Event(uint32_t event_no, uint8_t event_type):
    event_no(event_no), event_type(event_type)
{}

uint32_t Pixel::raw_data_len() { return 9; }
uint32_t PlayerEliminated::raw_data_len() { return 1; }
uint32_t GameOver::raw_data_len() { return 0; }
uint32_t NewGame::raw_data_len() {
    uint32_t raw_data_length = 8;
    for (char* name: players_names_list)
        raw_data_length += strlen(name) + 1; /* names separated by \0 */
    return raw_data_length;
}

char* Pixel::raw_data() {
    char* result = new char[raw_data_len()];
    result[0] = player_number;
    uint32_t net_x = htonl(x), net_y = htonl(y); /* network bytes order */
    memcpy(result + 1, &net_x, 4);
    memcpy(result + 5, &net_y, 4);
    return result;
}

Event *Pixel::parse_single_event_data(uint32_t event_no, char *event_data, size_t event_data_length) {
    if (event_data_length == 9) {
        uint8_t player_num;
        uint32_t x, y;
        memcpy(&player_num, event_data, 1);
        memcpy(&x, event_data + 1, 4);
        memcpy(&y, event_data + 5, 4);
        x = ntohl(x);
        y = ntohl(y);

        return new Pixel(event_no, player_num, x, y);
    }

    return nullptr;
}

char* PlayerEliminated::raw_data() {
    char* result = new char[raw_data_len()];
    result[0] = player_number;
    return result;
}

Event* PlayerEliminated::parse_single_event_data(uint32_t event_no, char *event_data, size_t event_data_length) {
    if (event_data_length == 1) {
        uint8_t player_num;
        memcpy(&player_num, event_data, 1);
        return new PlayerEliminated(event_no, player_num);
    }

    return nullptr;
}

char* GameOver::raw_data() {
    return nullptr;
}

Event *GameOver::parse_single_event_data(uint32_t event_no, size_t event_data_length) {
    if (event_data_length == 0)
        return new GameOver(event_no);
    return nullptr;
}


char* NewGame::raw_data() {

    uint32_t net_maxx = htonl(maxx), net_maxy = htonl(maxy); /* network bytes order */

    char* result = new char[raw_data_len()];
    char* current_ptr = result;
    memcpy(current_ptr, &net_maxx, 4);
    current_ptr += 4;
    memcpy(current_ptr, &net_maxy, 4);
    current_ptr += 4;
    uint32_t current_length = 8;

    /* name separated by '\0' */
    for (char* name: players_names_list) {
        uint32_t name_length = (uint32_t) strlen(name);
        memcpy(result + current_length, name, name_length);
        current_length += name_length;
        result[current_length] = '\0';
        current_length++;
    }
    return result;
}

Event *NewGame::parse_single_event_data(uint32_t event_no, char *event_data, size_t event_data_length) {
    if (event_data_length < 9) /* Not enough for maxx, maxy */
        return nullptr;

    if (event_data[event_data_length - 1] != '\0') /* each name should be ended with this */
        return nullptr;

    uint32_t maxx, maxy;
    char* current_ptr = event_data;
    memcpy(&maxx, current_ptr, 4);
    current_ptr += 4;
    memcpy(&maxy, current_ptr, 4);
    current_ptr += 4;
    maxx = ntohl(maxx);
    maxy = ntohl(maxy);

    vector<char*> player_names;

    size_t name_len;

    while (current_ptr != event_data + event_data_length) {
        name_len = strlen(current_ptr);

        if (name_len == 0 || name_len > MAX_NAME_LENGTH) /* names length are supposed to be in range [1, 64] for active players */
            return nullptr;
        char** player_name = new char*;
        *player_name = new char[MAX_NAME_LENGTH];
        memcpy(*player_name, current_ptr, name_len + 1);
        current_ptr += name_len + 1;
        player_names.push_back(*player_name);
    }

    return new NewGame(event_no, maxx, maxy, player_names);
}

uint32_t Event::event_raw_data_len() {
    return 13 + raw_data_len();
}

char* Event::event_raw_data() {
    char* result = new char[event_raw_data_len()];
    uint32_t net_len = htonl(event_raw_data_len() - 8); /* without len and csrc */
    uint32_t net_event_no = htonl(event_no); /* network bytes order */
    uint8_t event_type_var = get_event_type();
    memcpy(result, &net_len, 4);
    memcpy(result + 4, &net_event_no, 4);
    memcpy(result + 8, &event_type_var, 1);
    memcpy(result + 9, raw_data(), raw_data_len());

    uint32_t checksum = crc32(result, event_raw_data_len() - 4);
    uint32_t net_crc = htonl(checksum); /* without checksum itself */
    memcpy(result + event_raw_data_len() - 4, &net_crc, 4); /* append calculated checksum */
    return result;
}

vector<Event *> &Event::parse_events(char *event_data, size_t len) {
    uint32_t events_len, event_no;
    uint8_t event_type;
    uint32_t crc32_value;
    uint32_t event_length; /* the length of whole single event data */
    uint32_t event_data_length; /* length of the event data */
    vector<Event*>* res = new vector<Event*>;
    char* current_ptr = event_data;

    size_t remained_length = len;
    /* 13 bytes for data without event_data */
    while (current_ptr != event_data + len && remained_length > 13) {
        memcpy(&events_len, current_ptr, 4);
        current_ptr += 4;
        remained_length -= 4;
        memcpy(&event_no, current_ptr, 4);
        current_ptr += 4;
        remained_length -= 4;
        memcpy(&event_type, current_ptr, 1);
        current_ptr ++;
        remained_length--;
        events_len = ntohl(events_len);
        event_no = ntohl(event_no);

        if (events_len < 5)
            syserr("Wrong event len");

        event_data_length = events_len - 5; /* minus len(events_len) and len(event_no) */
        event_length = events_len + 8; /* plus len(events_len) and len(crc32) */
        memcpy(&crc32_value, current_ptr + event_data_length, 4);
        crc32_value = ntohl(crc32_value);

        if (crc32(current_ptr - 9, event_length - 4) != crc32_value)
            break;

        if (event_type > 3) /* skipping unknown type, though with sane data and correct checksum */
            continue;


        Event* resulted_event =
                parse_single_event_data(event_no, event_type, current_ptr, event_data_length);

        current_ptr += 4;

        current_ptr += event_data_length;

        remained_length -= event_data_length;
        remained_length -= 4;

        if (resulted_event != nullptr) /* correct event */
            (*res).push_back(resulted_event);
        else /* something got wrong in parsing event_data */
            break;
    }


    return (*res);
}

Event *Event::parse_single_event_data(uint32_t event_no, uint8_t event_type,
                                      char *event_data, size_t event_data_length) {
    switch (event_type) {
        case 0:
            return NewGame::parse_single_event_data(event_no, event_data, event_data_length);
        case 1:
            return Pixel::parse_single_event_data(event_no, event_data, event_data_length);
        case 2:
            return PlayerEliminated::parse_single_event_data(event_no, event_data, event_data_length);
        case 3:
            return GameOver::parse_single_event_data(event_no, event_data_length);
        default:
            return nullptr;
    }
}

