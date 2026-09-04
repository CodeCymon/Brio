// Copyright (c) Simon Kirsch 2026.


#include "Engine.h"


int main(int argc, char* argv[]) {
    Engine engine;
    if (engine.Initialize()) {
        engine.Run();
    }
    engine.Shutdown();

    return 0;
}
