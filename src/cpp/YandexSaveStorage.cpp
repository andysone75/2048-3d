#include "YandexSaveStorage.h"

#include "JS.h"

#include <sstream>
#include <string>


bool YandexSaveStorage::tryLoadData() {
	const char* dataC = js::getGameData();

	if (dataC == nullptr) return false;
	std::string data(dataC);
	if (data == "") return false;

	if (data == "{}") {
		setLoadCompleteFlag();
		return false;
	}

	bestScore = std::make_unique<int>();
	historyPointer = std::make_unique<int>();
	historyTree = std::make_unique<std::vector<HistoryTreeNode>>();

	std::istringstream in(data.c_str());

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

void YandexSaveStorage::save(const SaveData& data) {
	if (!isLoadComplete()) return;

    std::ostringstream out;

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

	js::saveGameData(out.str().c_str());
}