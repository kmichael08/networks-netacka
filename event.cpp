//
// Created by michal on 03.06.17.
//

#include "event.h"

NewGame::NewGame(uint32_t event_no, uint32_t maxx, uint32_t maxy, vector<string>& players_names_list) :
    Event(event_no), maxx(maxx), maxy(maxy), players_names_list(players_names_list)
{
    event_type = 0;
}

Pixel::Pixel(uint32_t event_no, uint8_t player_number, uint32_t x, uint32_t y) :
    Event(event_no), player_number(player_number), x(x), y(y)
{
    event_type = 1;
}

PlayerEliminated::PlayerEliminated(uint32_t event_no, uint8_t player_number) :
    Event(event_no), player_number(player_number)
{
    event_type = 2;
}

GameOver::GameOver(uint32_t event_no):
    Event(event_no)
{
    event_type = 3;
}

Event::Event(uint32_t event_no):
    event_no(event_no)
{}

