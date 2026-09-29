#pragma once

#include <vulkan/vulkan_core.h>

class FErolyssaPhysicalDevice;

class FErolyssaFence
{
    
public:
    FErolyssaFence(FErolyssaPhysicalDevice& InDevice);
    ~FErolyssaFence();
    
    void WaitFor();
    void Reset();
    VkFence GetHandle() const { return Handle; }

private:
    FErolyssaPhysicalDevice& Device;
    
    VkFence Handle = VK_NULL_HANDLE;
};
