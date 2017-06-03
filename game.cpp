#include "game.h"

Board::Board(uint32_t width, uint32_t height) :
width(width), height(height)
{
    board = new uint32_t*[width];
    for (uint32_t i = 0; i < width; i++)
        board[i] = new uint32_t[height];
}

bool Board::is_occupied(uint32_t x, uint32_t y) {
    return board[x][y] > 0;
}

Game::Game(uint32_t game_id) : game_id(game_id) {
}
