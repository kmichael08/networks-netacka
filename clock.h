#ifndef NETWORKS_NETACKA_CLOCK_H
#define NETWORKS_NETACKA_CLOCK_H

#include <cstdint>
#include <ctime>

class Clock {
    time_t last_turn_start_time;
    uint32_t next_turn_time;
    uint32_t average_turn_time;
public:
    Clock(uint32_t turn_time);
    bool end_turn() const;
    void next_turn(); /* reset last_turn_start_time */
};


#endif //NETWORKS_NETACKA_CLOCK_H
