
#include <cstring>
#include "client_utils.h"

int8_t take_direction_from_gui_message(char* message, size_t len) {
    len--; /* Ignore last character */
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
    cout << mess << " " << "MESS" << endl;
    printf("%s DATAGRAM sent\n", message);
    return new Datagram(message, mess.length() + 1);
}

Datagram* message_sent_to_gui(Event* event, vector<char*>& players_names) {
    uint8_t event_type = event->get_event_type();
    string mess;
    NewGame* newGame = nullptr;
    Pixel* pixel = nullptr;
    PlayerEliminated* playerEliminated = nullptr;

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
            mess.append(players_names.at(pixel->get_player_number()));
            return datagram_out_of_string(mess);
        case 2:
            playerEliminated = (PlayerEliminated*) event;
            mess.append("PLAYER_ELIMINATED ");
            mess.append(players_names.at(playerEliminated->get_player_number()));
            return datagram_out_of_string(mess);
        case 3:
            return nullptr;
        default:
            return nullptr;
    }
    return nullptr;
}
