#include "datagramClientToServer.h"
#include "event.h"
#include <cstring>
#include <netinet/in.h>

DatagramClientToServer::DatagramClientToServer(uint64_t session_id, int8_t turn_direction,
                                                   uint32_t next_expected_event_no, char* player_name) :
        session_id(session_id), turn_direction(turn_direction),
        next_expected_event_no(next_expected_event_no), player_name(player_name)
{}

DatagramClientToServer::DatagramClientToServer(char *raw_data, size_t len) {
    char* current_ptr = raw_data;
    memcpy(&session_id, current_ptr, 8);
    session_id = be64toh(session_id); /* network to host bytes order */
    current_ptr += 8;
    memcpy(&turn_direction, current_ptr, 1);
    current_ptr++;
    memcpy(&next_expected_event_no, current_ptr, 4);
    next_expected_event_no = ntohl(next_expected_event_no); /* network to host bytes order */
    current_ptr += 4;
    player_name = new char[len - 13];
    memcpy(player_name, current_ptr, len - 13);
    if (len == 13)
        no_name = true;
}

uint64_t DatagramClientToServer::get_session_id() { return session_id; }
uint32_t DatagramClientToServer::get_next_expected_event_no() { return next_expected_event_no; }
char* DatagramClientToServer::get_player_name() { return player_name; }
int8_t DatagramClientToServer::get_turn_direction() { return turn_direction; }
bool DatagramClientToServer::no_player_name() { return no_name; }

bool DatagramClientToServer::is_valid() {
    if (turn_direction < -1 || turn_direction > 1)
        return false; /* Wrong turn direction */

    return valid_player_name(player_name);
}

bool DatagramClientToServer::valid_name_charachter(char c) {
    return int(c) >= 33 && int(c) <= 126;
}

bool DatagramClientToServer::valid_player_name(char *player_name_arg) {
    if (strlen(player_name_arg) > NewGame::MAX_NAME_LENGTH)
        return false; /* Player name is too long */

    for (size_t i = 0; i < strlen(player_name_arg); i++)
        if (!valid_name_charachter(player_name_arg[i]))
            return false; /* invalid characters */

    return true;
}

Datagram *DatagramClientToServer::get_raw_datagram() {
    size_t len = 13 + strlen(player_name);
    char* datagram = new char[len];
    char* current_ptr = datagram;
    uint64_t net_session_id = htobe64(session_id);
    memcpy(current_ptr, &net_session_id, 8);
    current_ptr += 8;
    memcpy(current_ptr, &turn_direction, 1);
    current_ptr++;
    uint32_t net_next_exp_event_no = htonl(next_expected_event_no);
    memcpy(current_ptr, &net_next_exp_event_no, 4);
    current_ptr += 4;
    memcpy(current_ptr, player_name, strlen(player_name));
    current_ptr += strlen(player_name);
    return new Datagram(datagram, len);
}


