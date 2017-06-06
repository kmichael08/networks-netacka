#include <cstring>
#include <netinet/in.h>
#include "datagramServerToClient.h"

/* TODO NEW_GAME event may not fit into datagram alone - fix it */
Datagram* DatagramServerToClient::make_datagram(VEIT start, VEIT end, uint32_t size) {
    char* datagram = new char[size];
    uint32_t current_position = 0;
    uint32_t net_game_id = htonl(game_id);
    memcpy(datagram, &net_game_id, 4);
    current_position += 4;
    for (VEIT iter = start; iter != end; iter++) {
        uint32_t event_length = (*iter)->event_raw_data_len();
        memcpy(datagram + current_position, (*iter)->event_raw_data(), event_length);
        current_position += event_length;
    }

    return new Datagram(datagram, current_position);
}


vector<Datagram *> DatagramServerToClient::datagrams() {
    vector<Datagram*> result;
    vector<Event*>::iterator start = events.begin(), iter = events.begin();
    uint32_t current_size = 0;

    while (iter != events.end()) {
        uint32_t next_len = (*iter)->event_raw_data_len();
        if (current_size + next_len + 4 <= MAX_DATAGRAM_SIZE) {
            current_size += next_len;
            iter++;
        }
        else {
            result.push_back(make_datagram(start, iter, current_size));
            current_size = next_len;
            start = iter;
            iter++;
        }
    }

    if (start != iter) {
        result.push_back(make_datagram(start, iter, current_size));
    }

    return result;
}

DatagramServerToClient::DatagramServerToClient(uint32_t game_id, vector<Event *> &events) :
    game_id(game_id), events(events)
{}

Datagram::Datagram(char *data, size_t len): data(data), len(len) {}

char *Datagram::get_data() {
    return data;
}

size_t Datagram::get_len() const {
    return len;
}
