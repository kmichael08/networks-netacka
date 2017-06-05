#include "game.h"
#include <cmath>

Board::Board(uint32_t width, uint32_t height) :
width(width), height(height)
{
    board = new uint8_t*[width];
    for (uint32_t i = 0; i < width; i++)
        board[i] = new uint8_t[height](); /* TODO check, but this should init with zeros */
}

bool Board::is_occupied(uint32_t x, uint32_t y) {
    return board[x][y] == 1;
}

void Board::take(uint32_t x, uint32_t y) { board[x][y] = 1; }

uint32_t Board::get_height() { return height; }
uint32_t Board::get_width() { return width; }

Game::Game(uint32_t game_id, uint32_t width, uint32_t height, vector<Player*>& players,
           uint32_t turn_time, uint32_t turning_speed) :
        game_id(game_id), board(new Board(width, height)), players(players),
        turn_time(turn_time), turning_speed(turning_speed)
{}

double Player::get_direction() { return direction; }

Player::Player(uint32_t session_id, char* name): session_id(session_id), name(name),
                                                 last_activity_time(uint32_t(time(NULL))) {}

bool Player::not_responding() {
    return time(NULL) - last_activity_time >= TWO_SECS_IN_MICROSECS;
}

double Player::get_headx() { return headx; }

double Player::get_heady() { return heady; }

char* Player::get_name() { return name; }

uint32_t Player::get_session_id() { return session_id; }

void Player::set_parameters(double headx, double heady, double direction) {
    this->headx = headx;
    this->heady = heady;
    this->direction = direction;
}

bool Player::is_alive() { return alive; }

void Player::reborn() { alive = true; }

void Player::kill() { alive = false; }

void Player::turn(int8_t turn_direction, uint32_t turning_speed) {
    direction = (uint32_t)(direction + turn_direction * turning_speed + MAX_DIRECTION) % MAX_DIRECTION;
}

/* TODO check the correctness of these formula */
void Player::move() {
    headx += cos(direction);
    heady -= sin(direction);
}

bool Board::inside_board(double x, double y) {
    return (x >= 0) && (y >= 0) && (x < width) && (y < height);
}

Board* Game::get_board() { return board; }

void Game::add_game_over() {
    all_events.push_back(new GameOver((uint32_t)all_events.size()));
}

void Game::add_new_game(uint32_t width, uint32_t height, vector<char*> &players_names) {
    all_events.push_back(new NewGame((uint32_t)all_events.size(), width, height, players_names));
}

void Game::add_pixel(uint8_t player_number,uint32_t x, uint32_t y) {
    all_events.push_back(new Pixel((uint32_t)all_events.size(), player_number, x, y));
}

void Game::add_player_eliminated(uint8_t player_number) {
    all_events.push_back(new PlayerEliminated((uint32_t)all_events.size(), player_number));
    players[player_number]->kill(); /* Eliminated player is no longer alive */
}

bool Game::end_game() {
    int players_alive = 0;
    for (Player* player: players)
        players_alive += player->is_alive();
    return players_alive <= 1;
}

void Game::move_snake(int8_t turn_direction, Player *player, uint8_t player_num) {
    player->turn(turn_direction, turning_speed);
    player->move();
    double x = player->get_headx();
    double y = player->get_heady();

    if (!board->inside_board(x, y)) {
        add_player_eliminated(player_num);
    }
    if (board->is_occupied(uint32_t(x), (uint32_t)(y))) {
        add_player_eliminated(player_num);
    }
    else {
        board->take((uint32_t)x, (uint32_t)y);
        add_pixel(player_num, (uint32_t)x, (uint32_t)y);
    }

}