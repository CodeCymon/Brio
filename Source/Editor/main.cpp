
#include "Engine.h"

int main() {

    Engine Engine;
    bool bInitialized = Engine.Initialize({
        .width = 1280,
        .height = 720,
        .title = "Engine"
    });

    if (bInitialized) {
        Engine.Run();
    }

    Engine.Shutdown();
}
