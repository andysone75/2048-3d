#pragma once

#include <array>
#include <vector>

struct TileMove {
    int fromX, fromY;
    int toX, toY;
    bool merged;
};

struct HistoryTreeNode {
    int spawnX;
    int spawnY;

    int indexParent = -1;
    int indexChildUp = -1;
    int indexChildRight = -1;
    int indexChildDown = -1;
    int indexChildLeft = -1;
};

enum class MoveDirection { Up, Right, Down, Left };

class Game2048 {
public:
    Game2048(int maxLevel);

    void reset();
    bool goLeft();
    bool goRight();
    bool goUp();
    bool goDown();
    void undoMove();
    void setHistory(const std::vector<HistoryTreeNode>& historyTree, int historyPointer);

    const std::array<std::array<int, 4>, 4>& getBoard() const;
    const std::array<std::array<int, 4>, 4>& getPreviousBoard() const;
    const std::vector<TileMove>& getLastMoves() const;
    const std::vector<HistoryTreeNode>& getHistoryTree() const;
    const int& getHistoryPointer() const;

    int getScore() const;

private:
    int maxLevel;
    int score = 0;
    int historyPointer = 0;
    std::array<std::array<int, 4>, 4> board;
    std::array<std::array<int, 4>, 4> boardBeforeMove;
    std::vector<TileMove> lastMoves;
    std::vector<HistoryTreeNode> historyTree;

    void fixLastMovesAfterRotation(int times90Degrees);
    void rotateClockwise();
    void rotateCounterClockwise();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void addRandom(int& posX, int& posY);
    void add(int posX, int posY);
    bool boardChanged() const;
    void onGoComplete(MoveDirection moveDirection, int& spawnX, int& spawnY);
    void rebuildBoard();
};
