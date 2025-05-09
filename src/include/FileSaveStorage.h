#pragma once

#include "SaveStorage.h"

class FileSaveStorage : public SaveStorage {
public:
	void load() override;
	void save(const SaveData& data) override;
};