#include "Application.h"
#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

int main() {
    Resources resources;
    Scene scene(resources);
    Game2048 game(11);
    View2048 view(game, scene);

    Application app(resources, scene, game, view);

    if (!app.initialize()) {
        return 1;
    }

    resources.initialize();
    scene.initialize();
    app.initGame();

#ifdef __EMSCRIPTEN__
    auto callback = [](void* arg) {
        Application* pApp = reinterpret_cast<Application*>(arg);
        pApp->mainLoop();
        };
    emscripten_set_main_loop_arg(callback, &app, 0, true);
#else
    while (app.isRunning()) {
        app.mainLoop();
    }
#endif

    app.terminate();
    return 0;
}