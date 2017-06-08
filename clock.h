#ifndef NETWORKS_NETACKA_CLOCK_H
#define NETWORKS_NETACKA_CLOCK_H

#include <cstdint>
#include <ctime>

class Clock {
    /* all times in microseconds */
    uint64_t last_turn_start_time;
    uint64_t next_turn_time;
    uint64_t average_turn_time;
public:
    /**
     * @param turn_time in microseconds
     */
    Clock(uint64_t turn_time);
    bool end_turn() const;
    void next_turn(); /* reset last_turn_start_time */
};


#endif //NETWORKS_NETACKA_CLOCK_H
