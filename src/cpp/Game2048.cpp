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

void Game2048::reset() {
    score = 0;
    boardBeforeMove = {};
    lastMoves.clear();
    board = {};
    historyPointer = 0;

    int spawnX, spawnY;
    addRandom(spawnX, spawnY);

    historyTree.clear();
    historyTree.emplace_back();

    HistoryTreeNode& node = historyTree[0];
    node.spawnX = spawnX;
    node.spawnY = spawnY;
}

bool Game2048::goLeft() {
    boardBeforeMove = board;
    moveLeft();

    if (boardChanged()) {
        int spawnX, spawnY;
        onGoComplete(MoveDirection::Left, spawnX, spawnY);
        add(spawnX, spawnY);
        return true;
    }

    return false;
}

bool Game2048::goRight() {
    boardBeforeMove = board;
    moveRight();

    if (boardChanged()) {
        int spawnX, spawnY;
        onGoComplete(MoveDirection::Right, spawnX, spawnY);
        add(spawnX, spawnY);
        return true;
    }

    return false;
}

bool Game2048::goUp() {
    boardBeforeMove = board;
    moveUp();

    if (boardChanged()) {
        int spawnX, spawnY;
        onGoComplete(MoveDirection::Up, spawnX, spawnY);
        add(spawnX, spawnY);
        return true;
    }

    return false;
}

bool Game2048::goDown() {
    boardBeforeMove = board;
    moveDown();

    if (boardChanged()) {
        int spawnX, spawnY;
        onGoComplete(MoveDirection::Down, spawnX, spawnY);
        add(spawnX, spawnY);
        return true;
    }

    return false;
}

void Game2048::onGoComplete(MoveDirection moveDirection, int& spawnX, int& spawnY) {
    HistoryTreeNode& currentNode = historyTree[historyPointer];

    int childIndex = -1;
    switch (moveDirection) {
    case MoveDirection::Up:
        childIndex = currentNode.indexChildUp;
        break;
    case MoveDirection::Right:
        childIndex = currentNode.indexChildRight;
        break;
    case MoveDirection::Down:
        childIndex = currentNode.indexChildDown;
        break;
    case MoveDirection::Left:
        childIndex = currentNode.indexChildLeft;
        break;
    }

    if (childIndex != -1) {
        historyPointer = childIndex;
        HistoryTreeNode& newNode = historyTree[historyPointer];
        spawnX = newNode.spawnX;
        spawnY = newNode.spawnY;
    }
    else {
        addRandom(spawnX, spawnY);

        historyTree.emplace_back();
        int newNodeIndex = historyTree.size() - 1;

        HistoryTreeNode& currentNode = historyTree[historyPointer];

        switch (moveDirection)
        {
        case MoveDirection::Up:
            currentNode.indexChildUp = newNodeIndex;
            break;
        case MoveDirection::Right:
            currentNode.indexChildRight = newNodeIndex;
            break;
        case MoveDirection::Down:
            currentNode.indexChildDown = newNodeIndex;
            break;
        case MoveDirection::Left:
            currentNode.indexChildLeft = newNodeIndex;
            break;
        }

        HistoryTreeNode& newNode = historyTree[newNodeIndex];
        newNode.spawnX = spawnX;
        newNode.spawnY = spawnY;
        newNode.indexParent = historyPointer;

        historyPointer = newNodeIndex;
    }
}

void Game2048::moveLeft() {
    lastMoves.clear();
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
                lastMoves.push_back({ i, fromCol, i, targetCol, true });
                lastMoves.push_back({ i, tiles[j + 1].second, i, targetCol, true });
                ++j;
            }
            else {
                lastMoves.push_back({ i, fromCol, i, targetCol, false });
            }

            newRow[targetCol++] = value;
        }

        for (int j = 0; j < 4; ++j) {
            board[i][j] = newRow[j];
        }
    }
}

void Game2048::moveRight() {
    rotateClockwise();
    rotateClockwise();
    moveLeft();
    rotateClockwise();
    rotateClockwise();
    fixLastMovesAfterRotation(2);
}

void Game2048::moveUp() {
    rotateCounterClockwise();
    moveLeft();
    rotateClockwise();
    fixLastMovesAfterRotation(1);
}

void Game2048::moveDown() {
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

void Game2048::addRandom(int& posX, int& posY) {
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

    posX = x;
    posY = y;
}

void Game2048::add(int posX, int posY) {
    if (board[posX][posY] == 0)
        board[posX][posY] = 1;
}

void Game2048::undoMove() {
    if (historyTree[historyPointer].indexParent == -1)
        return;

    historyPointer = historyTree[historyPointer].indexParent;
    rebuildBoard();
}

void Game2048::rebuildBoard() {
    score = 0;
    boardBeforeMove = {};
    lastMoves.clear();
    board = {};

    int currentNode = historyPointer;
    std::vector<int> path;

    while (currentNode != -1) {
        path.push_back(currentNode);
        currentNode = historyTree[currentNode].indexParent;
    }

    std::reverse(path.begin(), path.end());

    for (const int& nodeIndex : path) {
        HistoryTreeNode& node = historyTree[nodeIndex];
        if (node.indexParent != -1) {
            HistoryTreeNode& parent = historyTree[node.indexParent];
            if (nodeIndex == parent.indexChildUp) moveUp();
            else if (nodeIndex == parent.indexChildRight) moveRight();
            else if (nodeIndex == parent.indexChildDown) moveDown();
            else if (nodeIndex == parent.indexChildLeft) moveLeft();
        }
        add(node.spawnX, node.spawnY);
    }
}

void Game2048::setHistory(const std::vector<HistoryTreeNode>& historyTree, int historyPointer) {
    if (historyTree.size() > 0) {
        this->historyTree = historyTree;
        this->historyPointer = historyPointer;
        rebuildBoard();
    }
}

const std::vector<TileMove>& Game2048::getLastMoves() const { return lastMoves; }
const std::array<std::array<int, 4>, 4>& Game2048::getPreviousBoard() const { return boardBeforeMove; }
bool Game2048::boardChanged() const { return board != boardBeforeMove; }
int Game2048::getScore() const { return score; }
const std::array<std::array<int, 4>, 4>& Game2048::getBoard() const { return board; }
const std::vector<HistoryTreeNode>& Game2048::getHistoryTree() const { return historyTree; }
const int& Game2048::getHistoryPointer() const { return historyPointer; }