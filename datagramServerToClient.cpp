#include <cstring>
#include <netinet/in.h>
#include <cassert>
#include "datagramServerToClient.h"

/* TODO NEW_GAME event may not fit into datagram alone - fix it */
Datagram* DatagramServerToClient::make_datagram(VEIT start, VEIT end, uint32_t size) {
    char* datagram = new char[size + 4]; /* 4 additional bytes for game_id */
    uint32_t current_position = 0;
    uint32_t net_game_id = htonl(game_id);
    memcpy(datagram, &net_game_id, 4);
    current_position += 4;
    for (VEIT iter = start; iter != end; iter++) {
        cout << (unsigned long)(*iter)->get_event_type() << " TYPE OF SENT EVENT" << endl;
        uint32_t event_length = (*iter)->event_raw_data_len();
        cout << event_length << " EVENT LENGTH " << endl;
        memcpy(datagram + current_position, (*iter)->event_raw_data(), event_length);
        current_position += event_length;
    }

    assert(current_position == size + 4);

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

DatagramServerToClient *DatagramServerToClient::parse_datagram(char* datagram, size_t len) {
    assert(len > 4);
    uint32_t parsed_game_id;
    memcpy(&parsed_game_id, datagram, 4);
    parsed_game_id = ntohl(parsed_game_id);
    return new DatagramServerToClient(parsed_game_id, Event::parse_events(datagram + 4, len - 4));
}

uint32_t DatagramServerToClient::get_game_id() const {
    return game_id;
}

vector<Event *>& DatagramServerToClient::get_events() {
    return events;
}


Datagram::Datagram(char *data, size_t len): data(data), len(len) {}

char *Datagram::get_data() {
    return data;
}

size_t Datagram::get_len() const {
    return len;
}
