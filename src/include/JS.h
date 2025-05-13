#pragma once

namespace js {
	void saveGameData(const char* data);
	const char* getGameData();

	void showFullscreenAdv();
	void showRewardedVideo();

	void setLeaderboardScore(int score);
	void gameReadyApi_ready();
	void loadSound(const char* filepath);
	void playSound(const char* filepath);

	void updatePurchases();
	void purchase(const char* id);
	bool hasPurchase(const char* id);
	const char* getProductPrice(const char* id);

	bool yandexInitialized();
	bool getRewardedAdCompleteFlag();
	bool getRewardedAdCloseFlag();
	bool getPurchasesUpdateFlag();
	bool getPurchaseCompleteFlag();

	void resetFlagsRewardedAd();
	void resetPurchasesUpdateFlag();
	void resetPurchaseCompleteFlag();
}