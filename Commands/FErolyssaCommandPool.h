// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../ErolyssaAliases.h"
#include "../Core/ErolyssaDevice.h"

enum class EErolyssaCommandPoolCreateFlags : uint8
{
    None                 = 0,
    Transient            = 1 << 0,  // буферы живут недолго — драйвер может оптимизировать
    ResetCommandBuffer   = 1 << 1,  // разрешить vkResetCommandBuffer на отдельных буферах
    Protected            = 1 << 2,  // защищённая память (для DRM)
};
ENUM_CLASS_FLAGS(EErolyssaCommandPoolCreateFlags);

enum class EErolyssaCommandBufferLevel : uint8
{
    Primary   = 0,
    Secondary = 1,
};

enum class EErolyssaCommandPoolResetFlags : uint8
{
    None             = 0,
    ReleaseResources = 1 << 0,
};
ENUM_CLASS_FLAGS(EErolyssaCommandPoolResetFlags)

class FErolyssaCommandPool
{
    
public:
    FErolyssaCommandPool(
        const FErolyssaDevice& InDevice,
        const FErolyssaQueueFamily InQueueFamily,
        const EErolyssaCommandPoolCreateFlags InFlags = EErolyssaCommandPoolCreateFlags::ResetCommandBuffer
    ) : Device(InDevice) , QueueFamily(InQueueFamily)
    {
        VkCommandPoolCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        CreateInfo.flags = ToVulkan(InFlags);
        CreateInfo.queueFamilyIndex = InQueueFamily;
        
        check(vkCreateCommandPool(Device, &CreateInfo, nullptr, &Handle) == VK_SUCCESS, "FErolyssaCommandPool: failed to create command pool!");
    }
    ~FErolyssaCommandPool()
    {
        if(Handle != VK_NULL_HANDLE) vkDestroyCommandPool(Device, Handle, nullptr);
    }
    
    FErolyssaCommandPool(const FErolyssaCommandPool&) = delete;
    FErolyssaCommandPool& operator=(const FErolyssaCommandPool&) = delete;
    
    operator VkCommandPool() const { return Handle; }
    
    const FErolyssaDevice& GetDevice() const { return Device; }
    FErolyssaQueueFamily GetQueueFamily() const { return QueueFamily; }
    
    VkCommandBuffer Allocate(const EErolyssaCommandBufferLevel InLevel = EErolyssaCommandBufferLevel::Primary) const
    {
        VkCommandBufferAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocInfo.commandPool = Handle;
        AllocInfo.level = ToVulkan(InLevel);
        AllocInfo.commandBufferCount = 1;
        
        VkCommandBuffer Buffer = VK_NULL_HANDLE;
        check(vkAllocateCommandBuffers(Device, &AllocInfo, &Buffer) == VK_SUCCESS, "FErolyssaCommandPool: failed to allocate command buffer!");
        return Buffer;
    }
    
    void Free(const VkCommandBuffer InBuffer) const { vkFreeCommandBuffers(Device, Handle, 1, &InBuffer); }
    void Reset(const EErolyssaCommandPoolResetFlags InFlags = EErolyssaCommandPoolResetFlags::None) const { vkResetCommandPool(Device, Handle, ToVulkan(InFlags)); }

private:
    const FErolyssaDevice& Device;
    VkCommandPool Handle = VK_NULL_HANDLE;
    FErolyssaQueueFamily QueueFamily = 0xFFFFFFFF;
    
    static VkCommandPoolCreateFlags ToVulkan(const EErolyssaCommandPoolCreateFlags InFlags)
    {
        VkCommandPoolCreateFlags Result = 0;
        if(EnumHasAnyFlags(InFlags, EErolyssaCommandPoolCreateFlags::Transient)) Result |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        if(EnumHasAnyFlags(InFlags, EErolyssaCommandPoolCreateFlags::ResetCommandBuffer)) Result |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        if(EnumHasAnyFlags(InFlags, EErolyssaCommandPoolCreateFlags::Protected)) Result |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
        return Result;
    }
    
    static VkCommandBufferLevel ToVulkan(const EErolyssaCommandBufferLevel InLevel)
    {
        switch(InLevel)
        {
        case EErolyssaCommandBufferLevel::Primary:   return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        case EErolyssaCommandBufferLevel::Secondary: return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        }
        return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    }
    
    static VkCommandPoolResetFlags ToVulkan(const EErolyssaCommandPoolResetFlags InFlags)
    {
        VkCommandPoolResetFlags Result = 0;
        if(EnumHasAnyFlags(InFlags, EErolyssaCommandPoolResetFlags::ReleaseResources)) Result |= VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;
        return Result;
    }
};
