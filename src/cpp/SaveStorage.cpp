#include "SaveStorage.h"

void SaveStorage::unload() {
	bestScore.reset();
	historyPointer.reset();
	historyTree.reset();
	loadFlag = false;
}

bool SaveStorage::checkLoaded() {
	bool loaded = loadFlag;
	loadFlag = false;
	return loaded;
}

void SaveStorage::setLoadFlag() {
	loadFlag = true;
}