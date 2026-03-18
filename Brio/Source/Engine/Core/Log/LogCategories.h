#pragma once

#include "Common/Defines.h"

namespace Log
{
    struct BAPI Category
    {
        explicit Category(const char* name);
        const char* name {nullptr};
    };
}

#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) \
struct BAPI LogCategory##CategoryName : public Log::Category \
{ \
    inline LogCategory##CategoryName() : Log::Category(#CategoryName) {} \
}; \
extern BAPI LogCategory##CategoryName Log##CategoryName;


#define DEFINE_LOG_CATEGORY(CategoryName) BAPI LogCategory##CategoryName Log##CategoryName;


#define DEFINE_LOG_CATEGORY_STATIC(CategoryName) \
static struct LogCategory##CategoryName : public Log::Category \
{ \
    inline LogCategory##CategoryName() : Log::Category(#CategoryName) {} \
} Log##CategoryName;



DECLARE_LOG_CATEGORY_EXTERN(Temp)
DECLARE_LOG_CATEGORY_EXTERN(Assertion)
DECLARE_LOG_CATEGORY_EXTERN(Core)
DECLARE_LOG_CATEGORY_EXTERN(Engine)
DECLARE_LOG_CATEGORY_EXTERN(Renderer)
DECLARE_LOG_CATEGORY_EXTERN(RHI)
DECLARE_LOG_CATEGORY_EXTERN(Scene)
DECLARE_LOG_CATEGORY_EXTERN(Assets)