// Root Engine / Erolyssa

#include "ErolyssaPipeline.h"

#include "../Core/ErolyssaDevice.h"

FErolyssaPipeline::~FErolyssaPipeline()
{
    if(PipelineHandle != VK_NULL_HANDLE) vkDestroyPipeline(Device, PipelineHandle, nullptr);
    if(PipelineLayoutHandle != VK_NULL_HANDLE) vkDestroyPipelineLayout(Device, PipelineLayoutHandle, nullptr);
    if(DescriptorSetLayoutHandle != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(Device, DescriptorSetLayoutHandle, nullptr);
}
