#include "game.h"

Board::Board(uint32_t width, uint32_t height) :
width(width), height(height)
{
    board = new uint8_t*[width];
    for (uint32_t i = 0; i < width; i++)
        board[i] = new uint8_t[height];
}

bool Board::is_occupied(uint32_t x, uint32_t y) {
    return board[x][y] == 1;
}

Game::Game(uint32_t game_id, uint32_t width, uint32_t height, vector<Player*>& players, uint32_t turn_time) :
        game_id(game_id), board(new Board(width, height)), players(players), turn_time(turn_time)
{}

double Player::get_direction() { return direction; }

Player::Player(uint32_t session_id, string name): session_id(session_id), name(name) {}

double Player::get_headx() { return headx; }

double Player::get_heady() { return heady; }

string Player::get_name() { return name; }

uint32_t Player::get_session_id() { return session_id; }

void Player::set_parameters(double headx, double heady, double direction) {
    this->headx = headx;
    this->heady = heady;
    this->direction = direction;
}

bool Player::is_alive() { return alive; }

void Player::reborn() { alive = true; }