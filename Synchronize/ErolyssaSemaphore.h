// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Core/ErolyssaDevice.h"

class FErolyssaSemaphore
{
public:
    explicit FErolyssaSemaphore(const FErolyssaDevice& InDevice) : Device(InDevice)
    {
        VkSemaphoreCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        check(vkCreateSemaphore(Device, &CreateInfo, nullptr, &Handle) == VK_SUCCESS, "FErolyssaSemaphore: failed to create semaphore!");
    }
    
    ~FErolyssaSemaphore()
    {
        if(Handle != VK_NULL_HANDLE) vkDestroySemaphore(Device, Handle, nullptr);
    }
    
    FErolyssaSemaphore(const FErolyssaSemaphore&) = delete;
    FErolyssaSemaphore& operator=(const FErolyssaSemaphore&) = delete;
    
    operator VkSemaphore() const { return Handle; }
    operator const VkSemaphore*() const { return &Handle; }

private:
    const FErolyssaDevice& Device;
    VkSemaphore Handle = VK_NULL_HANDLE;
};
