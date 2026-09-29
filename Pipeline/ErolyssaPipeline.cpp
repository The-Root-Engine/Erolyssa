// Root Engine / Erolyssa

#include "ErolyssaPipeline.h"
#include "ErolyssaPhysicalDevice.h"
#include <cstdio>
#include <cstdlib>

#include "Erolyssa.h"

FErolyssaPipeline::~FErolyssaPipeline()
{
    if(PipelineHandle != VK_NULL_HANDLE) vkDestroyPipeline(Device.GetLogicalHandle(), PipelineHandle, nullptr);
    if(PipelineLayoutHandle != VK_NULL_HANDLE) vkDestroyPipelineLayout(Device.GetLogicalHandle(), PipelineLayoutHandle, nullptr);
    if(DescriptorSetLayoutHandle != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(Device.GetLogicalHandle(), DescriptorSetLayoutHandle, nullptr);
}
