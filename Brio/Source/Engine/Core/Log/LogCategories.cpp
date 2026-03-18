#include "LogCategories.h"

DEFINE_LOG_CATEGORY(Temp)
DEFINE_LOG_CATEGORY(Assertion)
DEFINE_LOG_CATEGORY(Core)
DEFINE_LOG_CATEGORY(Engine)
DEFINE_LOG_CATEGORY(Renderer)
DEFINE_LOG_CATEGORY(RHI)
DEFINE_LOG_CATEGORY(Scene)
DEFINE_LOG_CATEGORY(Assets)

Log::Category::Category(const char* name)
{
    this->name = name;
}
