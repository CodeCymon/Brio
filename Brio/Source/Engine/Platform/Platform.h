#pragma once
#include "Common/Defines.h"

class Platform
{
public:
    BAPI void initialize();
    BAPI void shutdown();

    BAPI void update();

    BAPI [[nodiscard]] bool closeRequested() const;
};
