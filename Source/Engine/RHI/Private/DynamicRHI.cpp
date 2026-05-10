#include "DynamicRHI.h"

#include "VulkanRHI/VulkanRHI.h"
#include "Log/Log.h"
#include "Log/Assert.h"
#include "Memory/References.h"

DEFINE_LOG_CATEGORY(LogRHI);

IDynamicRHI* GDynamicRHI = nullptr;

namespace RHI {
    static Unique<IDynamicRHI> SDynamicRHIInstance  {nullptr};

    void Create(FNativeWindowHandle const &windowHandle) {
        SDynamicRHIInstance = NewUnique<VulkanRHI>();
        GDynamicRHI = SDynamicRHIInstance.get();
        SDynamicRHIInstance->Initialize(windowHandle);
    }
}

void RHI::Destroy() {
    SDynamicRHIInstance->Shutdown();
    SDynamicRHIInstance.reset();
    GDynamicRHI = nullptr;
}

IDynamicRHI & RHI::Get() {
    ASSERT(SDynamicRHIInstance);
    return *SDynamicRHIInstance;
}
