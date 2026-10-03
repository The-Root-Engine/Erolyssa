// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Core/ErolyssaDevice.h"

class FErolyssaFence
{
    
public:
    FErolyssaFence(const FErolyssaDevice& InDevice, const bool bSignaled = false) : Device(InDevice)
    {
        VkFenceCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        CreateInfo.flags = bSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        
        check(vkCreateFence(Device, &CreateInfo, nullptr, &Handle) == VK_SUCCESS, "FErolyssaFence: failed to create fence!");
    }
    
    ~FErolyssaFence()
    {
        if(Handle != VK_NULL_HANDLE) vkDestroyFence(Device, Handle, nullptr);
    }
    
    FErolyssaFence(const FErolyssaFence&) = delete;
    FErolyssaFence& operator=(const FErolyssaFence&) = delete;
    
    operator VkFence() const { return Handle; }
    operator const VkFence*() const { return &Handle; }
    
    void Wait(const uint64 InTimeout = UINT64_MAX) const { vkWaitForFences(Device, 1, &Handle, VK_TRUE, InTimeout); }
    void Reset() const { vkResetFences(Device, 1, &Handle); }
    bool IsSignaled() const { return vkGetFenceStatus(Device, Handle) == VK_SUCCESS; }

private:
    const FErolyssaDevice& Device;
    VkFence Handle = VK_NULL_HANDLE;
};
