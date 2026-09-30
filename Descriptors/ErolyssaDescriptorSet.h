#pragma once

#include <vulkan/vulkan_core.h>

class FErolyssaDescriptorSet
{
    friend struct FErolyssaDescriptorPoolBuilder;
    
public:
    FErolyssaDescriptorSet() = default;
    ~FErolyssaDescriptorSet() = default;
    
    VkDescriptorSet& GetHandle() { return Handle; }

private:
    VkDescriptorSet Handle = VK_NULL_HANDLE;
};
