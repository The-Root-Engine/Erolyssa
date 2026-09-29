// Root Engine / Erolyssa

#pragma once

#include <vulkan/vulkan_core.h>

class FErolyssaPhysicalDevice;

class FErolyssaPipeline
{
    friend class FErolyssaPipelineBuilder;
    friend struct FErolyssaDescriptorPoolBuilder;
    
public:
    FErolyssaPipeline(FErolyssaPhysicalDevice& InDevice) : Device(InDevice) {}
    ~FErolyssaPipeline();
    
    operator VkPipeline() const { return PipelineHandle; }
    operator VkPipelineLayout() const { return PipelineLayoutHandle; }
    operator VkDescriptorSetLayout() const { return DescriptorSetLayoutHandle; }

private:
    FErolyssaPhysicalDevice& Device;
    
    VkPipeline PipelineHandle = VK_NULL_HANDLE;
    VkPipelineLayout PipelineLayoutHandle = VK_NULL_HANDLE;
    VkDescriptorSetLayout DescriptorSetLayoutHandle = VK_NULL_HANDLE;
};
