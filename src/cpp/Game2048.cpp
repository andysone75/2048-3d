#include "Game2048.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

Game2048::Game2048() {
    for (auto& row : board)
        row.fill(0);
}

const std::array<std::array<int, 4>, 4>& Game2048::getBoard() const {
    return board;
}

void Game2048::setBoard(const std::array<std::array<int, 4>, 4>& newBoard) {
    board = newBoard;
}

void Game2048::goLeft() {
    moveLeft();
}

void Game2048::goRight() {
    rotateClockwise();
    rotateClockwise();
    moveLeft();
    rotateClockwise();
    rotateClockwise();
}

void Game2048::goUp() {
    rotateCounterClockwise();
    moveLeft();
    rotateClockwise();
}

void Game2048::goDown() {
    rotateClockwise();
    moveLeft();
    rotateCounterClockwise();
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
    for (int i = 0; i < 4; ++i) {
        std::vector<int> row;
        for (int j = 0; j < 4; ++j)
            if (board[i][j] != 0)
                row.push_back(board[i][j]);

        for (int j = 0; j + 1 < row.size(); ++j) {
            if (row[j] == row[j + 1]) {
                row[j]++;
                row[j + 1] = 0;
            }
        }

        std::vector<int> newRow;
        for (int val : row)
            if (val != 0)
                newRow.push_back(val);

        while (newRow.size() < 4)
            newRow.push_back(0);

        for (int j = 0; j < 4; ++j)
            board[i][j] = newRow[j];
    }
}

void Game2048::printBoard() const {
    for (const auto& row : board) {
        for (int val : row)
            std::cout << val << "\t";
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
