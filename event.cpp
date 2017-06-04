#include "event.h"
#include <cstring>
#include <netinet/in.h>

NewGame::NewGame(uint32_t event_no, uint32_t maxx, uint32_t maxy, vector<char*>& players_names_list) :
    Event(event_no), maxx(maxx), maxy(maxy), players_names_list(players_names_list)
{
    event_type = 0;
}

Pixel::Pixel(uint32_t event_no, uint8_t player_number, uint32_t x, uint32_t y) :
    Event(event_no), player_number(player_number), x(x), y(y)
{
    event_type = 1;
}

PlayerEliminated::PlayerEliminated(uint32_t event_no, uint8_t player_number) :
    Event(event_no), player_number(player_number)
{
    event_type = 2;
}

GameOver::GameOver(uint32_t event_no):
    Event(event_no)
{
    event_type = 3;
}

Event::Event(uint32_t event_no):
    event_no(event_no)
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

char* PlayerEliminated::raw_data() {
    char* result = new char[raw_data_len()];
    result[0] = player_number;
    return result;
}

char* GameOver::raw_data() {
    return nullptr;
}

char* NewGame::raw_data() {

    uint32_t net_maxx = htonl(maxx), net_maxy = htonl(maxy); /* network bytes order */

    char* result = new char[raw_data_len()];
    memcpy(result, &net_maxx, 4);
    memcpy(result, &net_maxy, 4);

    uint32_t current_length = 8;

    /* ame separated by '\0' */
    for (char* name: players_names_list) {
        uint32_t name_length = (uint32_t) strlen(name);
        memcpy(result + current_length, name, name_length);
        current_length += name_length;
        result[current_length] = '\0';
        current_length++;
    }
    return result;
}

uint32_t Event::event_raw_data_len() {
    return 13 + raw_data_len();
}

char* Event::event_raw_data() {
    char* result = new char[event_raw_data_len()];
    uint32_t net_len = htonl(event_raw_data_len() - 8); /* without len and csrc */
    uint32_t net_event_no = htonl(event_no); /* network bytes order */
    memcpy(result, &net_len, 4);
    memcpy(result + 4, &net_event_no, 4);
    memcpy(result + 8, &event_type, 1);
    memcpy(result + 9, raw_data(), raw_data_len());
    uint32_t net_csrc = htonl(csrc32(result, event_raw_data_len() - 4)); /* withour checksum itself */
    memcpy(result + event_raw_data_len() - 4, &net_csrc, 4); /* append calculated checksum */
    return result;
}