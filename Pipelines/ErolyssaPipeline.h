// Root Engine / Erolyssa

#pragma once

#include <vulkan/vulkan_core.h>

class FErolyssaDevice;

class FErolyssaPipeline
{
    friend class FErolyssaPipelineBuilder;
    friend struct FErolyssaDescriptorPoolBuilder;
    
public:
    FErolyssaPipeline(FErolyssaDevice& InDevice) : Device(InDevice) {}
    ~FErolyssaPipeline();
    
    operator VkPipeline() const { return PipelineHandle; }
    operator VkPipelineLayout() const { return PipelineLayoutHandle; }
    operator VkDescriptorSetLayout() const { return DescriptorSetLayoutHandle; }

private:
    FErolyssaDevice& Device;
    
    VkPipeline PipelineHandle = VK_NULL_HANDLE;
    VkPipelineLayout PipelineLayoutHandle = VK_NULL_HANDLE;
    VkDescriptorSetLayout DescriptorSetLayoutHandle = VK_NULL_HANDLE;
};
