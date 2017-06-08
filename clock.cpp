#include "clock.h"
#include "utils.h"

Clock::Clock(uint64_t turn_time): next_turn_time(turn_time), average_turn_time(turn_time) {
    last_turn_start_time = get_time_microseconds();
}

bool Clock::end_turn() const {
    uint64_t delta = 1000; /* 1ms */
    /* if the time from the last is larger than the turn time (with delta) */
    return (get_time_microseconds() - last_turn_start_time >= next_turn_time - delta);
}

void Clock::next_turn() {
    uint64_t temp = get_time_microseconds();
    uint64_t passed = temp - last_turn_start_time; /* used time */
    int64_t delta = next_turn_time - passed; /* how much left from the previous turn, may be negative */
    next_turn_time = average_turn_time + delta;
}
