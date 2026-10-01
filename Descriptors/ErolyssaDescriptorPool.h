// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Core/ErolyssaDevice.h"

struct FErolyssaDescriptorPoolSize
{
    VkDescriptorType Type  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uint32_t         Count = 1;
};

class FErolyssaDescriptorPool
{
    
public:
    FErolyssaDescriptorPool(
        const FErolyssaDevice& InDevice,
        const std::initializer_list<FErolyssaDescriptorPoolSize> InSizes,
        const uint32_t InMaxSets = 1,
        VkDescriptorPoolCreateFlags InFlags = 0
    ) : Device(InDevice)
    {
        std::vector<VkDescriptorPoolSize> VkSizes;
        VkSizes.reserve(InSizes.size());
        
        for(const auto& S : InSizes)
        {
            VkDescriptorPoolSize VkS{};
            VkS.type = S.Type;
            VkS.descriptorCount = S.Count;
            VkSizes.push_back(VkS);
        }
        
        VkDescriptorPoolCreateInfo Info{};
        Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        Info.flags = InFlags;
        Info.poolSizeCount = static_cast<uint32_t>(VkSizes.size());
        Info.pPoolSizes = VkSizes.data();
        Info.maxSets = InMaxSets;
        
        check(vkCreateDescriptorPool(InDevice, &Info, nullptr, &Handle) == VK_SUCCESS,
              "FErolyssaDescriptorPool: failed to create!");
    }
    
    ~FErolyssaDescriptorPool()
    {
        if(Handle != VK_NULL_HANDLE) vkDestroyDescriptorPool(Device, Handle, nullptr);
    }
    
    FErolyssaDescriptorPool(const FErolyssaDescriptorPool&) = delete;
    FErolyssaDescriptorPool& operator=(const FErolyssaDescriptorPool&) = delete;
    
    operator VkDescriptorPool() const { return Handle; }
    bool IsValid() const { return Handle != VK_NULL_HANDLE; }

private:
    const FErolyssaDevice& Device;
    VkDescriptorPool Handle = VK_NULL_HANDLE;
};
