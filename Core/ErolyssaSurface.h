// Root Engine / Erolyssa

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class FErolyssaInstance;

class FErolyssaPhysicalDevice
{
public:
    FErolyssaPhysicalDevice(const FErolyssaInstance& InInstance);
    ~FErolyssaPhysicalDevice();
    
    VkPhysicalDevice GetPhysicalHandle() const { return PhysicalDeviceHandle; }
    VkDevice GetLogicalHandle() const { return DeviceHandle; }
    VkQueue GetComputeQueue() const { return ComputeQueueHandle; }
    uint32_t GetComputeQueueFamilyIndex() const { return ComputeQueueFamilyIndex; }

private:
    VkPhysicalDevice PhysicalDeviceHandle = VK_NULL_HANDLE;
    VkDevice DeviceHandle = VK_NULL_HANDLE;
    VkQueue ComputeQueueHandle = VK_NULL_HANDLE;
    
    uint32_t ComputeQueueFamilyIndex = 0xFFFFFFFF;
    
    void PickPhysicalDevice(VkInstance InstanceHandle);
    void CreateLogicalDevice();
    
    int RateDeviceSuitability(VkPhysicalDevice Device);
    uint32_t FindComputeQueueFamily(VkPhysicalDevice Device);
};
