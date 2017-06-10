#include "game.h"
#include <cmath>
#include <cassert>

Board::Board(uint32_t width, uint32_t height) :
width(width), height(height)
{
    board = new uint8_t*[width];
    for (uint32_t i = 0; i < width; i++)
        board[i] = new uint8_t[height](); /* this should init with zeros */
}

bool Board::is_occupied(uint32_t x, uint32_t y) {
    return board[x][y] == 1;
}

void Board::take(uint32_t x, uint32_t y) { board[x][y] = 1; }

uint32_t Board::get_height() { return height; }
uint32_t Board::get_width() { return width; }

Game::Game(uint32_t game_id, uint32_t width, uint32_t height, vector<Player*>& players,
           uint64_t turn_time, uint32_t turning_speed) :
        game_id(game_id), board(new Board(width, height)), players(players),
        turn_time(turn_time), turning_speed(turning_speed), clock(new Clock(turn_time))
{}

double Player::get_direction() { return direction; }

Player::Player(uint64_t session_id, char* name, sockaddr_in * client_address):
        session_id(session_id), name(name), client_address(client_address), last_activity_time(get_time_microseconds())
        {}

bool Player::not_responding() {
    uint64_t current = get_time_microseconds();
    return (current - last_activity_time >= TWO_SECS_IN_MICROSECS);
}

double Player::get_headx() { return headx; }

double Player::get_heady() { return heady; }

char* Player::get_name() { return name; }

uint64_t Player::get_session_id() { return session_id; }

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

void Player::move() {
    headx += cos(direction * M_PI / 180.);
    heady -= sin(direction * M_PI / 180.);
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

bool Game::game_ended() {
    int players_alive = 0;
    for (Player* player: players)
        players_alive += player->is_alive();
    return players_alive <= 1;
}

void Game::move_snake(int8_t turn_direction, Player *player) {
    uint8_t player_num = player->get_player_number();
    uint32_t initial_x = (uint32_t)player->get_headx();
    uint32_t initial_y = (uint32_t)player->get_heady();
    player->turn(turn_direction, turning_speed);
    player->move();
    double x = player->get_headx();
    double y = player->get_heady();
    uint32_t current_x = (uint32_t)x;
    uint32_t current_y = (uint32_t)y;
    if (current_x == initial_x && current_y == initial_y) /* Position has not changed */
        return;

    if (!board->inside_board(x, y)) {
        add_player_eliminated(player_num);
    }
    else if (board->is_occupied(current_x, current_y)) {
        add_player_eliminated(player_num);
    }
    else {
        board->take((uint32_t)x, (uint32_t)y);
        add_pixel(player_num, (uint32_t)x, (uint32_t)y);
    }

}

struct sockaddr_in* Player::get_client_address() { return client_address; }

bool Player::equal_address(sockaddr_in *second_address) {
    sockaddr_in* address = this->client_address;
    return address->sin_port == second_address->sin_port && address->sin_addr.s_addr == second_address->sin_addr.s_addr;
}

void Player::update() {
    last_activity_time = get_time_microseconds();
}

uint8_t Player::get_player_number() const { return player_number; }

void Player::set_player_number(uint8_t number) { player_number = number; }

Player::Player(Player *other_player) {
    Player(other_player->session_id, other_player->name, other_player->client_address);
}

uint32_t Game::get_game_id() const { return game_id; }

vector<Event *> Game::get_events_from(uint32_t first_event_no) {
    if (first_event_no < all_events.size())
        return vector<Event *>(all_events.begin() + first_event_no, all_events.end());
    else
        return vector<Event *>();
}

uint32_t Game::get_events_number() const {
    return (uint32_t)all_events.size();
}

void Game::end_game() {
    for (Player* player: players)
        player->kill();
    add_game_over();

}

void Game::move_snakes() {
    for (Player* player: players)
        if (player->is_alive())
            move_snake(player->get_last_turn_direction(), player);
}

bool Game::end_turn() const {
    return clock->end_turn();
}

void Game::next_turn() {
    clock->next_turn();
    for (Player* player: players)
        player->set_current_turn_direction(0);
}

void Player::set_current_turn_direction(int8_t turn_direction) { last_turn_direction = turn_direction;}

int8_t Player::get_last_turn_direction() const {
    return last_turn_direction;
}

