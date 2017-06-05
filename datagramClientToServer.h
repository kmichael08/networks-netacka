#ifndef NETWORKS_NETACKA_DATAGRAMCLIENTTOSERVER_H
#define NETWORKS_NETACKA_DATAGRAMCLIENTTOSERVER_H

#include <cstdint>
#include <iostream>

using namespace std;

class DatagramClientToServer {
private:
    uint64_t session_id; /* All datagrams from the client have it equal */
    int8_t turn_direction; /* in {-1, 0, 1}, -1 LEFT, +1 RIGHT, 0 STRAIGHT */
    uint32_t next_expected_event_no;
    char* player_name; /* 0-64 ASCII, in range[33, 126], empty - observer */
public:
    DatagramClientToServer(uint64_t session_id, int8_t turn_direction,
                           uint32_t next_expected_event_no, char* player_name);
    DatagramClientToServer(char* raw_data);
    uint64_t get_session_id();
    int8_t get_turn_direction();
    uint32_t get_next_expected_event_no();
    char* get_player_name();
    bool no_player_name();
    bool is_valid(); /* Is the datagram valid */
    static bool valid_name_charachter(char c);
};


#endif //NETWORKS_NETACKA_DATAGRAMCLIENTTOSERVER_H
