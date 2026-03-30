
#include <Engine/Engine.h>

int main() {
    Engine::Config config = {
        .width = 1280,
        .height = 720,
        .title = "Brio"
    };
    Engine engine(config);
    engine.init();
    engine.run();
    engine.shutdown();

    return 0;
}
