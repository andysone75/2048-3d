#include "JS.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#ifdef __EMSCRIPTEN__
EM_JS(void, _saveGameData, (const char* data), { saveGameData(UTF8ToString(data)); });
EM_JS(const char*, _getPlayerData, (), { return toCString(getPlayerData()); });

EM_JS(void, _showFullscreenAdv, (), { showFullscreenAdv(); });
EM_JS(void, _showRewardedVideo, (), { showRewardedVideo(); });

EM_JS(void, _setLeaderboardScore, (int score), { setLeaderboardScore(score); });
EM_JS(void, _gameReadyApi_ready, (), { gameReadyApi_ready(); });
EM_JS(const char*, _getLanguage, (), { return toCString(getLanguage()); });
EM_JS(void, _loadSound, (const char* filepath), { loadSound(UTF8ToString(filepath)); });
EM_JS(void, _playSound, (const char* filepath), { playSound(UTF8ToString(filepath)); });

EM_JS(void, _updatePurchases, (), { updatePurchases(); });
EM_JS(void, _purchase, (const char* id), { purchase(UTF8ToString(id)); });
EM_JS(bool, _hasPurchase, (const char* id), { return hasPurchase(UTF8ToString(id)); });
EM_JS(const char*, _getProductPrice, (const char* id), { return toCString(getProductPrice(UTF8ToString(id))); });

EM_JS(bool, _yandexInitialized, (), { return yandexInitialized() });
EM_JS(bool, _getRewardedAdCompleteFlag, (), { return getRewardedAdCompleteFlag() });
EM_JS(bool, _getRewardedAdCloseFlag, (), { return getRewardedAdCloseFlag() });
EM_JS(bool, _getPurchasesUpdateFlag, (), { return getPurchasesUpdateFlag() });
EM_JS(bool, _getPurchaseCompleteFlag, (), { return getPurchaseCompleteFlag() });

EM_JS(void, _resetFlagsRewardedAd, (), { return resetFlagsRewardedAd() });
EM_JS(void, _resetPurchasesUpdateFlag, (), { return resetPurchasesUpdateFlag() });
EM_JS(void, _resetPurchaseCompleteFlag, (), { return resetPurchaseCompleteFlag() });
#endif

namespace js {
	void saveGameData(const char* data) {
		#if __EMSCRIPTEN__
		_saveGameData(data);
		#endif
	}

	const char* getGameData() {
		#if __EMSCRIPTEN__
		return _getPlayerData();
		#endif
		return nullptr;
	}

	void showFullscreenAdv() {
		#if __EMSCRIPTEN__
		_showFullscreenAdv();
		#endif
	}

	void showRewardedVideo() {
		#if __EMSCRIPTEN__
		_showRewardedVideo();
		#endif
	}

	void setLeaderboardScore(int score) {
		#if __EMSCRIPTEN__
		_setLeaderboardScore(score);
		#endif
	}

	void gameReadyApi_ready() {
		#if __EMSCRIPTEN__
		_gameReadyApi_ready();
		#endif
	}

	const char* getLanguage()
	{
		#if __EMSCRIPTEN__
		return _getLanguage();
		#endif
		return "en";
	}

	void loadSound(const char* filepath) {
		#ifdef __EMSCRIPTEN__
		_loadSound(filepath);
		#endif
	}

	void playSound(const char* filepath) {
		#ifdef __EMSCRIPTEN__
		_playSound(filepath);
		#endif
	}

	void updatePurchases() {
		#if __EMSCRIPTEN__
		_updatePurchases();
		#endif
	}

	void purchase(const char* id) {
		#if __EMSCRIPTEN__
		_purchase(id);
		#endif
	}

	bool hasPurchase(const char* id) {
		#if __EMSCRIPTEN__
		return _hasPurchase(id);
		#endif
		return false;
	}

	const char* getProductPrice(const char* id) {
		#if __EMSCRIPTEN__
		return _getProductPrice(id);
		#endif
		return nullptr;
	}

	bool yandexInitialized() {
		#if __EMSCRIPTEN__
		return _yandexInitialized();
		#endif
		return false;
	}

	bool getRewardedAdCompleteFlag() {
		#if __EMSCRIPTEN__
		return _getRewardedAdCompleteFlag();
		#endif
		return false;
	}

	bool getRewardedAdCloseFlag() {
		#if __EMSCRIPTEN__
		return _getRewardedAdCloseFlag();
		#endif
		return false;
	}

	bool getPurchasesUpdateFlag() {
		#if __EMSCRIPTEN__
		return _getPurchasesUpdateFlag();
		#endif
		return false;
	}

	bool getPurchaseCompleteFlag() {
		#if __EMSCRIPTEN__
		return _getPurchaseCompleteFlag();
		#endif
		return false;
	}

	void resetFlagsRewardedAd() {
		#if __EMSCRIPTEN__
		_resetFlagsRewardedAd();
		#endif
	}

	void resetPurchasesUpdateFlag() {
		#if __EMSCRIPTEN__
		_resetPurchasesUpdateFlag();
		#endif
	}

	void resetPurchaseCompleteFlag() {
		#if __EMSCRIPTEN__
		_resetPurchaseCompleteFlag();
		#endif
	}
}