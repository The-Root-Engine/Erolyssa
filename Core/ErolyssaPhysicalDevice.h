// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Erolyssa.h"
#include "ErolyssaInstance.h"
#include "ErolyssaQueue.h"
#include "ErolyssaQueueFamily.h"

#include <cstring>
#include <malloc.h>

class FErolyssaPhysicalDevice
{
    
public:
    FErolyssaPhysicalDevice(const FErolyssaInstance& InInstance) { PickBest(InInstance); }
    ~FErolyssaPhysicalDevice() = default;
    
    FErolyssaPhysicalDevice(const FErolyssaPhysicalDevice&) = delete;
    FErolyssaPhysicalDevice& operator=(const FErolyssaPhysicalDevice&) = delete;
    
    operator VkPhysicalDevice() const { return Handle; }
    
    const VkPhysicalDeviceProperties& GetProperties() const { return Properties; }
    const VkPhysicalDeviceFeatures& GetFeatures() const { return Features; }
    
    uint32_t FindQueueFamily(VkQueueFlags InFlags = 0, VkSurfaceKHR InSurface = VK_NULL_HANDLE) const
    {
        uint32_t FamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(Handle, &FamilyCount, nullptr);
        check(FamilyCount > 0, "No queue families found!");
        VkQueueFamilyProperties* Families = EROLYSSA_VLA(VkQueueFamilyProperties, FamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(Handle, &FamilyCount, Families);
        
        for(uint32_t i = 0; i < FamilyCount; ++i)
        {
            if((Families[i].queueFlags & InFlags) != InFlags) continue;
            if(InSurface != VK_NULL_HANDLE && !IsSupportsPresent(InSurface, i)) continue;
            return i;
        }
        
        return 0xFFFFFFFF;
    }
    
    bool IsSupportsPresent(const VkSurfaceKHR InSurface, const FErolyssaQueueFamily InQueueFamily) const
    {
        VkBool32 bSupports = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(Handle, InQueueFamily, InSurface, &bSupports);
        return bSupports == VK_TRUE;
    }
    
    bool SupportsExtension(const char* InExtensionName) const
    {
        uint32_t ExtensionCount = 0;
        vkEnumerateDeviceExtensionProperties(Handle, nullptr, &ExtensionCount, nullptr);
        if(ExtensionCount == 0) return false;
        VkExtensionProperties* Extensions = EROLYSSA_VLA(VkExtensionProperties, ExtensionCount);
        vkEnumerateDeviceExtensionProperties(Handle, nullptr, &ExtensionCount, Extensions);
        
        for(uint32_t i = 0; i < ExtensionCount; ++i)
            if(strcmp(Extensions[i].extensionName, InExtensionName) == 0)
                return true;
        
        return false;
    }

private:
    VkPhysicalDevice Handle = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties Properties{};
    VkPhysicalDeviceFeatures Features{};
    
    void PickBest(const FErolyssaInstance& InInstance)
    {
        uint32_t DeviceCount = 0;
        vkEnumeratePhysicalDevices(InInstance, &DeviceCount, nullptr);
        check(DeviceCount > 0, "No Vulkan-capable GPU found!");
        VkPhysicalDevice* Devices = EROLYSSA_VLA(VkPhysicalDevice, DeviceCount);
        vkEnumeratePhysicalDevices(InInstance, &DeviceCount, Devices);
        
        int BestScore = -1;
        VkPhysicalDevice BestDevice = VK_NULL_HANDLE;
        
        for(uint32_t i = 0; i < DeviceCount; ++i)
            if(const int Score = RateSuitability(Devices[i]); Score > BestScore)
            {
                BestScore = Score;
                BestDevice = Devices[i];
            }
        
        check(BestDevice != VK_NULL_HANDLE, "No suitable GPU found!");
        
        Handle = BestDevice;
        vkGetPhysicalDeviceProperties(Handle, &Properties);
        vkGetPhysicalDeviceFeatures(Handle, &Features);
        
        FErolyssaDebug::Log("Selected GPU: %s", Properties.deviceName);
    }
    
    static uint32_t RateSuitability(const VkPhysicalDevice InPhysicalDevice)
    {
        VkPhysicalDeviceProperties Props{};
        vkGetPhysicalDeviceProperties(InPhysicalDevice, &Props);
        
        uint32_t Score = 0;
        
        /**/ if(Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) Score += 1000;
        else if(Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) Score += 500;
        else if(Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) Score += 100;
        else if(Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) Score += 10;
        
        Score += Props.limits.maxImageDimension2D / 1000;
        
        return Score;
    }
};
