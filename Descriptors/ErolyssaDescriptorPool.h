#pragma once

#include <vulkan/vulkan_core.h>

class FErolyssaPhysicalDevice;

struct FErolyssaDescriptorPool
{
    friend struct FErolyssaDescriptorPoolBuilder;
    
public:
    FErolyssaDescriptorPool(FErolyssaPhysicalDevice& InDevice) : Device(InDevice) {}
    ~FErolyssaDescriptorPool()
    {
        if(Handle != VK_NULL_HANDLE)
            vkDestroyDescriptorPool(Device.GetLogicalHandle(), Handle, nullptr);
    }
    
    VkDescriptorPool GetHandle() { return Handle; }

private:
    FErolyssaPhysicalDevice& Device;
    VkDescriptorPool Handle = VK_NULL_HANDLE;
};
