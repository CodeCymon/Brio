#include "Log/Log.h"
#include "Log/Assert.h"

int main() {
    LOG_INFO(LogCore, "Info Message");

    ASSERT(1+1 == 2);

    return 0;
}