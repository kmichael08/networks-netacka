#include "datagramClientToServer.h"
#include <cstring>

DatagramClientToServer::DatagramClientToServer(uint64_t session_id, int8_t turn_direction,
                                                   uint32_t next_expected_event_no, char* player_name) :
        session_id(session_id), turn_direction(turn_direction),
        next_expected_event_no(next_expected_event_no), player_name(player_name)
{}

DatagramClientToServer::DatagramClientToServer(char *raw_data) {
    char* current_ptr = raw_data;
    memcpy(&session_id, current_ptr, 8);
    current_ptr += 8;
    memcpy(&turn_direction, current_ptr, 1);
    current_ptr++;
    memcpy(&next_expected_event_no, current_ptr, 4);
    current_ptr += 4;
    memcpy(player_name, current_ptr, strlen(current_ptr));
}

uint64_t DatagramClientToServer::get_session_id() { return session_id; }
uint32_t DatagramClientToServer::get_next_expected_event_no() { return next_expected_event_no; }
char* DatagramClientToServer::get_player_name() { return player_name; }
int8_t DatagramClientToServer::get_turn_direction() { return turn_direction; }
bool DatagramClientToServer::no_player_name() { return strlen(player_name) == 0; }