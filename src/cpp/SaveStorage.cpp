#include "SaveStorage.h"

void SaveStorage::load() {
	loadFlag = true;
}

void SaveStorage::unload() {
	bestScore.reset();
	historyPointer.reset();
	historyTree.reset();
	loadFlag = false;
}

bool SaveStorage::checkLoaded() {
	if (!loadFlag) return false;
	bool loaded = tryLoadData();
	loadFlag = !loaded;
	return loaded;
}
