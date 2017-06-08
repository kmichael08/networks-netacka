
#include <cstring>
#include "client_utils.h"

int8_t take_direction_from_gui_message(char* message, size_t len) {
    if (strncmp(message, "LEFT_KEY_DOWN", len) == 0)
        return -1;
    if (strncmp(message, "LEFT_KEY_UP", len) == 0)
        return -1;
    if (strncmp(message, "RIGHT_KEY_DOWN", len) == 0)
        return 1;
    if (strncmp(message, "RIGHT_KEY_UP", len) == 0)
        return 1;
    return 0;
}

Datagram* datagram_out_of_string(string& mess) {
    char* message;
    message = new char[mess.length() + 1];
    strncpy(message, mess.c_str(), mess.length());
    message[mess.length()] = char(10);
    return new Datagram(message, mess.length() + 1);
}

Datagram* message_sent_to_gui(Event* event, char* player_name) {
    uint8_t event_type = event->get_event_type();
    string mess;
    NewGame* newGame = nullptr;
    Pixel* pixel = nullptr;

    switch (event_type) {
        case 0:
            newGame = (NewGame*) event;
            mess.append("NEW_GAME ");
            mess.append(get_string_of_32bit(newGame->get_maxx()));
            mess.append(" ");
            mess.append(get_string_of_32bit(newGame->get_maxy()));
            mess.append(" ");
            for (char* name: newGame->get_players_name_list()) {
                mess.append(name);
                mess.append(" ");
            }
            return datagram_out_of_string(mess);
        case 1:
            pixel = (Pixel*) event;
            mess.append("PIXEL ");
            mess.append(get_string_of_32bit(pixel->get_x()));
            mess.append(" ");
            mess.append(get_string_of_32bit(pixel->get_y()));
            mess.append(" ");
            mess.append(player_name);
            return datagram_out_of_string(mess);
        case 2:
            mess.append("PLAYER_ELIMINATED ");
            mess.append(player_name);
            return datagram_out_of_string(mess);
        case 3:
            return nullptr;
        default:
            return nullptr;
    }
    return nullptr;
}