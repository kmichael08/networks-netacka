#include "datagramClientToServer.h"

DatagramClientToServer::DatagramClientToServer(uint64_t session_id, int8_t turn_direction,
                                                   uint32_t next_expected_event_no, char* player_name) :
        session_id(session_id), turn_direction(turn_direction),
        next_expected_event_no(next_expected_event_no), player_name(player_name)
{}
