#include "Game2048.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

Game2048::Game2048(int maxLevel_) : maxLevel(maxLevel_) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            board[i][j] = 0;

    boardBeforeMove = board;
}

void Game2048::fixLastMovesAfterRotation(int times90Degrees) {
    for (auto& move : lastMoves) {
        for (int t = 0; t < times90Degrees; ++t) {
            int fromX = move.fromX, fromY = move.fromY;
            int toX = move.toX, toY = move.toY;
            
            move.fromX = fromY;
            move.fromY = 3 - fromX;
            move.toX = toY;
            move.toY = 3 - toX;
        }
    }
}

const std::array<std::array<int, 4>, 4>& Game2048::getBoard() const {
    return board;
}

void Game2048::setBoard(const std::array<std::array<int, 4>, 4>& newBoard) {
    board = newBoard;
}

void Game2048::goLeft() {
    boardBeforeMove = board;
    moveLeft();
}

void Game2048::goRight() {
    boardBeforeMove = board;
    rotateClockwise();
    rotateClockwise();
    moveLeft();
    rotateClockwise();
    rotateClockwise();
    fixLastMovesAfterRotation(2);
}

void Game2048::goUp() {
    boardBeforeMove = board;
    rotateCounterClockwise();
    moveLeft();
    rotateClockwise();
    fixLastMovesAfterRotation(1);
}

void Game2048::goDown() {
    boardBeforeMove = board;
    rotateClockwise();
    moveLeft();
    rotateCounterClockwise();
    fixLastMovesAfterRotation(3);
}

void Game2048::rotateClockwise() {
    std::array<std::array<int, 4>, 4> rotated;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            rotated[j][3 - i] = board[i][j];
    board = rotated;
}

void Game2048::rotateCounterClockwise() {
    for (int i = 0; i < 3; ++i)
        rotateClockwise();
}

void Game2048::moveLeft() {
    resetMoves();
    for (int i = 0; i < 4; ++i) {
        std::vector<std::pair<int, int>> tiles;
        
        for (int j = 0; j < 4; ++j) {
            if (board[i][j] != 0) {
                tiles.emplace_back(board[i][j], j);
            }
        }

        std::vector<int> newRow(4, 0);
        int targetCol = 0;
        
        for (size_t j = 0; j < tiles.size(); ++j) {
            int value = tiles[j].first;
            int fromCol = tiles[j].second;

            if (j + 1 < tiles.size() && value == tiles[j + 1].first && value < maxLevel) {
                value += 1;
                score += (1 << value);
                lastMoves.push_back({i, fromCol, i, targetCol, true});
                lastMoves.push_back({i, tiles[j + 1].second, i, targetCol, true});
                ++j;
            } else {
                lastMoves.push_back({i, fromCol, i, targetCol, false});
            }

            newRow[targetCol++] = value;
        }

        for (int j = 0; j < 4; ++j) {
            board[i][j] = newRow[j];
        }
    }
}


void Game2048::printBoard() const {
    std::cout << "Current board:\n";
    for (const auto& row : board) {
        for (int val : row) {
            if (val == 0) std::cout << ".\t";
            else std::cout << (1 << val) << "\t";
        }
        std::cout << "\n";
    }

    std::cout << "\nPrevious board:\n";
    for (const auto& row : boardBeforeMove) {
        for (int val : row) {
            if (val == 0) std::cout << ".\t";
            else std::cout << (1 << val) << "\t";
        }
        std::cout << "\n";
    }
}

void Game2048::addRandom() {
    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (board[i][j] == 0)
                emptyCells.emplace_back(i, j);

    if (emptyCells.empty()) return;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, emptyCells.size() - 1);

    auto [x, y] = emptyCells[dist(gen)];
    board[x][y] = 1;
}


void Game2048::resetMoves() {
    lastMoves.clear();
}

const std::vector<TileMove>& Game2048::getLastMoves() const {
    return lastMoves;
}

const std::array<std::array<int, 4>, 4>& Game2048::getPreviousBoard() const {
    return boardBeforeMove;
}

bool Game2048::boardChanged() const {
    return board != boardBeforeMove;
}

int Game2048::getScore() const {
    return score;
}

void Game2048::reset() {
    score = 0;
    board = {};
    boardBeforeMove = {};
    lastMoves.clear();
}