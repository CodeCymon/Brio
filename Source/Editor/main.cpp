


#include <Platform.h>
#include <Log/Log.h>


struct Foo {
    void close() {
        bRun = false;
        LOG_DEBUG(LogTemp, "Closing!");
    }

    void resize(u32 width, u32 height) {
        LOG_DEBUG(LogTemp, "Resized! {}:{}", width, height);
    }

    bool bRun = true;
};

int main() {

    Foo foo;

    Platform platform;
    platform.Initialize({});
    platform.OnCloseDelegate.Add(&foo, &Foo::close);
    platform.OnResizeDelegate.Add(&foo, &Foo::resize);

    while (foo.bRun) {
        platform.PollEvents();
    }

    platform.Shutdown();
}
