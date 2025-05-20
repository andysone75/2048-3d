#include "JS.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#ifdef __EMSCRIPTEN__
EM_JS(void, _loadSound, (const char* filepath), { loadSound(UTF8ToString(filepath)); });
EM_JS(void, _playSound, (const char* filepath), { playSound(UTF8ToString(filepath)); });
#endif

namespace js {
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
}