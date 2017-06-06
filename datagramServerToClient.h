#ifndef NETWORKS_NETACKA_DATAGRAMSERVERTOCLIENT_H
#define NETWORKS_NETACKA_DATAGRAMSERVERTOCLIENT_H


#include <cstdint>
#include <vector>
#include "event.h"
#include <iostream>

using namespace std;

using VEIT = vector<Event*>::iterator;

class Datagram {
    char* data;
    size_t len;
public:
    Datagram(char* data, size_t len);
    char* get_data();
    size_t get_len() const;
};

class DatagramServerToClient {
private:
    static const int MAX_DATAGRAM_SIZE = 512; /* in bytes */
    uint32_t game_id;
    vector<Event*> events;
    Datagram* make_datagram(VEIT start, VEIT end, uint32_t size);
public:
    DatagramServerToClient(uint32_t game_id, vector<Event*>& events);
    vector<Datagram*> datagrams(); /* datagrams splitted, so that they don't exceed MAX_DATAGRAM_SIZE */
    /* make datagram out of events from start to end of a given size */
};


#endif //NETWORKS_NETACKA_DATAGRAMSERVERTOCLIENT_H
