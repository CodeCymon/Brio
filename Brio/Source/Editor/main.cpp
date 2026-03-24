
#include <Platform/Platform.h>

int main() {
  Platform platform;
  platform.initialize();

  while (true) {
    platform.update();

    if (platform.closeRequested())
      break;
  }

  platform.shutdown();

  return 0;
}
