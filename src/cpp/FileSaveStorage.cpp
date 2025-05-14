#include "FileSaveStorage.h"
#include <fstream>

#define FILENAME "save.txt"

bool FileSaveStorage::tryLoadData() {
	bestScore = std::make_unique<int>();
	historyPointer = std::make_unique<int>();
	historyTree = std::make_unique<std::vector<HistoryTreeNode>>();

	std::ifstream in(FILENAME);

	if (!in.is_open()) {
		setLoadCompleteFlag();
		return false;
	}

	size_t treeSize;

	in >> *bestScore;
	in >> *historyPointer;
	in >> treeSize;

	historyTree->resize(treeSize);

	for (size_t i = 0; i < treeSize; ++i) {
		HistoryTreeNode& node = historyTree->at(i);
		in >> node.spawnX >> node.spawnY
			>> node.indexParent >> node.indexChildUp
			>> node.indexChildRight >> node.indexChildDown
			>> node.indexChildLeft;
	}

	setLoadCompleteFlag();
	return true;
}

void FileSaveStorage::save(const SaveData& data) {
	if (!isLoadComplete()) return;

	std::ofstream out(FILENAME);
	out << data.bestScore << '\n';
	out << data.historyPointer << '\n';
	out << data.historyTree.size() << '\n';
	for (const auto& node : data.historyTree) {
		out << node.spawnX << ' ' << node.spawnY << ' '
			<< node.indexParent << ' '
			<< node.indexChildUp << ' '
			<< node.indexChildRight << ' '
			<< node.indexChildDown << ' '
			<< node.indexChildLeft << '\n';
	}
}