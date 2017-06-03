//
// Created by michal on 03.06.17.
//

#include "event.h"

NewGame::NewGame(uint32_t maxx, uint32_t maxy, vector<string>& players_list) :
    maxx(maxx), maxy(maxy), players_list(players_list)
{}

uint8_t NewGame::event_no() { return 0; }

Pixel::Pixel(uint8_t player_number, uint32_t x, uint32_t y) :
    player_number(player_number), x(x), y(y)
{}

uint8_t Pixel::event_no() { return 1; }

PlayerEliminated::PlayerEliminated(uint8_t player_number) :
    player_number(player_number)
{}

uint8_t PlayerEliminated::event_no() { return 2; }

GameOver::GameOver() {}

uint8_t GameOver::event_no() { return 3; }