#pragma once

#include "SaveStorage.h"

class FileSaveStorage : public SaveStorage {
public:
	bool tryLoadData() override;
	void save(const SaveData& data) override;
};