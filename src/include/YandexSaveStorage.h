#pragma once

#include "SaveStorage.h"

class YandexSaveStorage : public SaveStorage {
public:
	bool tryLoadData() override;
	void save(const SaveData& data) override;
};