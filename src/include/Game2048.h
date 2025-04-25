#pragma once
#include <array>

class Game2048 {
private:
    std::array<std::array<int, 4>, 4> board;

    void rotateClockwise();
    void rotateCounterClockwise();
    void moveLeft();

public:
    Game2048();

    const std::array<std::array<int, 4>, 4>& getBoard() const;
    void setBoard(const std::array<std::array<int, 4>, 4>& newBoard);

    void goLeft();
    void goRight();
    void goUp();
    void goDown();
    void addRandom();

    void printBoard() const;
};