#ifndef NETWORKS_NETACKA_DATAGRAMSERVERTOCLIENT_H
#define NETWORKS_NETACKA_DATAGRAMSERVERTOCLIENT_H


#include <cstdint>
#include <vector>
#include "event.h"
#include <iostream>

using namespace std;

class DatagramServerToClient {
private:
    static const int MAX_DATAGRAM_SIZE = 512; /* in bytes */
    uint32_t game_id;
    vector<Event> events;
};


#endif //NETWORKS_NETACKA_DATAGRAMSERVERTOCLIENT_H
