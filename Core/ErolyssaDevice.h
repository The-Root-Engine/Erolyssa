// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../ErolyssaAliases.h"
#include "ErolyssaPhysicalDevice.h"

#include <set>

class FErolyssaDevice
{
    
public:
    explicit FErolyssaDevice(
        const FErolyssaPhysicalDevice& InPhysicalDevice, const VkSurfaceKHR& InSurface,
        const bool bNeedGraphics, const bool bNeedCompute, const bool bNeedTransfer,
        const std::vector<const char*>& InRequiredExtensions
    ) : PhysicalDevice(InPhysicalDevice)
    {
        check(InSurface != VK_NULL_HANDLE, "FErolyssaDevice: bNeedPresent = true, but surface is null!");
        
        if(bNeedGraphics)
        {
            GraphicsFamily = PhysicalDevice.FindQueueFamily(VK_QUEUE_GRAPHICS_BIT);
            check(GraphicsFamily != 0xFFFFFFFF, "FErolyssaDevice: no graphics queue family found!");
        }
        
        if(bNeedCompute)
        {
            ComputeFamily = PhysicalDevice.FindQueueFamily(VK_QUEUE_COMPUTE_BIT);
            check(ComputeFamily != 0xFFFFFFFF, "FErolyssaDevice: no compute queue family found!");
        }
        
        if(bNeedTransfer)
        {
            TransferFamily = PhysicalDevice.FindQueueFamily(VK_QUEUE_TRANSFER_BIT);
            check(TransferFamily != 0xFFFFFFFF, "FErolyssaDevice: no transfer queue family found!");
        }
        else
        {
            TransferFamily = GraphicsFamily;
        }
        
        PresentFamily = PhysicalDevice.FindQueueFamily(0, InSurface);
        check(PresentFamily != 0xFFFFFFFF, "FErolyssaDevice: no present queue family found!");
        
        /*
        if(PresentFamily != 0xFFFFFFFF)
        {
            /*#1# if(PresentFamily == GraphicsFamily) PresentQueue = GraphicsQueue;
            else if(PresentFamily == ComputeFamily) PresentQueue = ComputeQueue;
            else if(PresentFamily == TransferFamily) PresentQueue = TransferQueue;
            else vkGetDeviceQueue(Handle, PresentFamily, 0, &PresentQueue);
        }
        */
        
        std::set<uint32_t> UniqueFamilies;
        if(GraphicsFamily != 0xFFFFFFFF) UniqueFamilies.insert(GraphicsFamily);
        if(ComputeFamily  != 0xFFFFFFFF) UniqueFamilies.insert(ComputeFamily);
        if(TransferFamily != 0xFFFFFFFF) UniqueFamilies.insert(TransferFamily);
        if(PresentFamily  != 0xFFFFFFFF) UniqueFamilies.insert(PresentFamily);
        
        constexpr float Priority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> QueueInfos;
        QueueInfos.reserve(UniqueFamilies.size());
        
        for(const uint32_t Family : UniqueFamilies)
        {
            VkDeviceQueueCreateInfo QueueInfo{};
            QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            QueueInfo.queueFamilyIndex = Family;
            QueueInfo.queueCount = 1;
            QueueInfo.pQueuePriorities = &Priority;
            QueueInfos.push_back(QueueInfo);
        }
        
        std::vector<const char*> Extensions;
        Extensions.reserve(InRequiredExtensions.size());
        for(const char* Ext : InRequiredExtensions)
            Extensions.push_back(Ext);
        
        for(const char* Ext : Extensions)
            check(PhysicalDevice.SupportsExtension(Ext), "FErolyssaDevice: GPU does not support extension '%s'", Ext);
        
        VkPhysicalDeviceFeatures enabledFeatures{};
        enabledFeatures.geometryShader = VK_TRUE;
        
        VkDeviceCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        CreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueInfos.size());
        CreateInfo.pQueueCreateInfos = QueueInfos.data();
        CreateInfo.pEnabledFeatures = &enabledFeatures;
        CreateInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
        CreateInfo.ppEnabledExtensionNames = Extensions.empty() ? nullptr : Extensions.data();
        CreateInfo.enabledLayerCount = 0;
        CreateInfo.ppEnabledLayerNames = nullptr;
        
        check(vkCreateDevice(PhysicalDevice, &CreateInfo, nullptr, &Handle) == VK_SUCCESS, "FErolyssaDevice: failed to create logical device!");
        
        if(GraphicsFamily != 0xFFFFFFFF)
        {
            vkGetDeviceQueue(Handle, GraphicsFamily, 0, &GraphicsQueue);
        }
        
        if(PresentFamily != 0xFFFFFFFF)
        {
            /**/ if(PresentFamily == GraphicsFamily) PresentQueue = GraphicsQueue;
            else vkGetDeviceQueue(Handle, PresentFamily, 0, &PresentQueue);
        }
        
        if(ComputeFamily != 0xFFFFFFFF)
        {
            /**/ if(ComputeFamily == GraphicsFamily) ComputeQueue = GraphicsQueue;
            else vkGetDeviceQueue(Handle, ComputeFamily, 0, &ComputeQueue);
        }
        
        if(TransferFamily != 0xFFFFFFFF)
        {
            /**/ if(TransferFamily == GraphicsFamily) TransferQueue = GraphicsQueue;
            else if(TransferFamily == ComputeFamily) TransferQueue = ComputeQueue;
            else vkGetDeviceQueue(Handle, TransferFamily, 0, &TransferQueue);
        }
    }
    
    ~FErolyssaDevice()
    {
        if(Handle != VK_NULL_HANDLE) vkDestroyDevice(Handle, nullptr);
    }
    
    FErolyssaDevice(const FErolyssaDevice&) = delete;
    FErolyssaDevice& operator=(const FErolyssaDevice&) = delete;
    
    operator const FErolyssaPhysicalDevice&() const { return PhysicalDevice; }
    operator VkDevice() const { return Handle; }
    operator VkPhysicalDevice() const { return PhysicalDevice; }
    
    bool IsValid() const { return Handle != VK_NULL_HANDLE; }
    
    FErolyssaQueue GetGraphicsQueue() const { return GraphicsQueue; }
    FErolyssaQueue GetComputeQueue () const { return ComputeQueue ; }
    FErolyssaQueue GetTransferQueue() const { return TransferQueue; }
    FErolyssaQueue GetPresentQueue () const { return PresentQueue ; }
    
    FErolyssaQueueFamily GetGraphicsFamily() const { return GraphicsFamily; }
    FErolyssaQueueFamily GetComputeFamily () const { return ComputeFamily ; }
    FErolyssaQueueFamily GetTransferFamily() const { return TransferFamily; }
    FErolyssaQueueFamily GetPresentFamily () const { return PresentFamily ; }
    
    void WaitIdle() const { vkDeviceWaitIdle(Handle); }

private:
    const FErolyssaPhysicalDevice& PhysicalDevice;
    VkDevice Handle = VK_NULL_HANDLE;
    
    FErolyssaQueue GraphicsQueue = VK_NULL_HANDLE;
    FErolyssaQueue ComputeQueue  = VK_NULL_HANDLE;
    FErolyssaQueue TransferQueue = VK_NULL_HANDLE;
    FErolyssaQueue PresentQueue  = VK_NULL_HANDLE;
    
    FErolyssaQueueFamily GraphicsFamily = 0xFFFFFFFF;
    FErolyssaQueueFamily ComputeFamily  = 0xFFFFFFFF;
    FErolyssaQueueFamily TransferFamily = 0xFFFFFFFF;
    FErolyssaQueueFamily PresentFamily  = 0xFFFFFFFF;
};
