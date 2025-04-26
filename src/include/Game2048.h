#ifndef GAME2048_H
#define GAME2048_H

#include <array>
#include <vector>

struct TileMove {
    int fromX, fromY;
    int toX, toY;
    bool merged;
};

class Game2048 {
private:
    int maxLevel;
    std::array<std::array<int, 4>, 4> board;
    std::array<std::array<int, 4>, 4> boardBeforeMove;
    std::vector<TileMove> lastMoves;

    void fixLastMovesAfterRotation(int times90Degrees);
    void rotateClockwise();
    void rotateCounterClockwise();
    void moveLeft();
    void resetMoves();

public:
    Game2048(int maxLevel);

    const std::array<std::array<int, 4>, 4>& getBoard() const;
    const std::array<std::array<int, 4>, 4>& getPreviousBoard() const;
    const std::vector<TileMove>& getLastMoves() const;
    void setBoard(const std::array<std::array<int, 4>, 4>& newBoard);
    bool boardChanged() const;

    void goLeft();
    void goRight();
    void goUp();
    void goDown();

    void addRandom();
    void printBoard() const;
};

#endif // GAME2048_H
