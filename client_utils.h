
#ifndef NETWORKS_NETACKA_CLIENT_UTILS_H
#define NETWORKS_NETACKA_CLIENT_UTILS_H

#include "datagramServerToClient.h"

int8_t take_direction_from_gui_message(char* message, size_t len);

Datagram* datagram_out_of_string(string& mess);

Datagram* message_sent_to_gui(Event* event, vector<char*>& players_names);

#endif //NETWORKS_NETACKA_CLIENT_UTILS_H
