#pragma once

#include <memory>

#include "Game2048.h"

struct SaveData {
	int bestScore;
	const int& historyPointer;
	const std::vector<HistoryTreeNode>& historyTree;

	SaveData(
		const int& _bestScore,
		const int& _historyPointer,
		const std::vector<HistoryTreeNode>& _historyTree) :
		bestScore(_bestScore),
		historyPointer(_historyPointer),
		historyTree(_historyTree) {}
};

class SaveStorage {
public:
	virtual ~SaveStorage() = default;
	virtual void load() = 0;
	virtual void save(const SaveData& data) = 0;

	void unload();
	bool checkLoaded();

	inline int getBestScore() const { return *bestScore; }
	inline int getHistoryPointer() const { return *historyPointer; }
	inline const std::vector<HistoryTreeNode>& getHistoryTree() const { return *historyTree; }

protected:
	std::unique_ptr<int> bestScore;
	std::unique_ptr<int> historyPointer;
	std::unique_ptr<std::vector<HistoryTreeNode>> historyTree;
	void setLoadFlag();

private:
	bool loadFlag = false;
};