// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Core/ErolyssaDevice.h"

struct FErolyssaDescriptorBinding
{
    uint32_t             Binding         = 0;
    VkDescriptorType     Type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uint32_t             Count           = 1;
    VkShaderStageFlags   StageFlags      = VK_SHADER_STAGE_ALL;
    const VkSampler*     ImmutableSampler = nullptr;
};

class FErolyssaDescriptorSetLayout
{
    
public:
    FErolyssaDescriptorSetLayout(
        const FErolyssaDevice& InDevice,
        const std::initializer_list<FErolyssaDescriptorBinding> InBindings
    ) : Device(InDevice)
    {
        std::vector<VkDescriptorSetLayoutBinding> VkBindings;
        VkBindings.reserve(InBindings.size());
        
        for(const auto& B : InBindings)
        {
            VkDescriptorSetLayoutBinding VkB{};
            VkB.binding = B.Binding;
            VkB.descriptorType = B.Type;
            VkB.descriptorCount = B.Count;
            VkB.stageFlags = B.StageFlags;
            VkB.pImmutableSamplers = B.ImmutableSampler;
            VkBindings.push_back(VkB);
        }
        
        VkDescriptorSetLayoutCreateInfo Info{};
        Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        Info.bindingCount = static_cast<uint32_t>(VkBindings.size());
        Info.pBindings = VkBindings.data();
        
        check(vkCreateDescriptorSetLayout(InDevice, &Info, nullptr, &Handle) == VK_SUCCESS, "FErolyssaDescriptorSetLayout: failed to create!");
    }
    ~FErolyssaDescriptorSetLayout()
    {
        if(Handle != VK_NULL_HANDLE)  vkDestroyDescriptorSetLayout(Device, Handle, nullptr);
    }
    
    FErolyssaDescriptorSetLayout(const FErolyssaDescriptorSetLayout&) = delete;
    FErolyssaDescriptorSetLayout& operator=(const FErolyssaDescriptorSetLayout&) = delete;
    
    operator VkDescriptorSetLayout() const { return Handle; }
    operator const VkDescriptorSetLayout*() const { return &Handle; }
    bool IsValid() const { return Handle != VK_NULL_HANDLE; }

private:
    const FErolyssaDevice& Device;
    VkDescriptorSetLayout Handle = VK_NULL_HANDLE;
};
