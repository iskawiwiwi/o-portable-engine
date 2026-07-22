#include "Engine.h"

int main(int argc, char* argv[]) {
    Engine engine;

    if (engine.Init("O Engine - Architecture 1.0", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        engine.Run();
    }

    return 0;
}
