// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Erolyssa.h"
#include "../Core/ErolyssaDevice.h"

class FErolyssaPipeline
{
    
public:
    FErolyssaPipeline(const FErolyssaDevice& InDevice)
        : Device(InDevice) {}
    
    FErolyssaPipeline(const FErolyssaDevice& InDevice, const VkPipelineLayoutCreateInfo& InPipelineLayoutInfo, VkGraphicsPipelineCreateInfo& InGraphicsPipelineInfo)
        : Device(InDevice) { Initialize(InPipelineLayoutInfo, InGraphicsPipelineInfo); }
    
    FErolyssaPipeline(const FErolyssaDevice& InDevice, const VkPipelineLayoutCreateInfo& InPipelineLayoutInfo, VkComputePipelineCreateInfo& InComputePipelineInfo)
        : Device(InDevice) { Initialize(InPipelineLayoutInfo, InComputePipelineInfo); }
    
    ~FErolyssaPipeline()
    {
        if(PipelineHandle) vkDestroyPipeline(Device, PipelineHandle, nullptr);
        if(PipelineLayoutHandle) vkDestroyPipelineLayout(Device, PipelineLayoutHandle, nullptr);
    }
    
    FErolyssaPipeline(const FErolyssaPipeline&) = delete;
    FErolyssaPipeline& operator=(const FErolyssaPipeline&) = delete;
    
    operator VkPipelineLayout() const { return PipelineLayoutHandle; }
    operator const VkPipelineLayout*() const { return &PipelineLayoutHandle; }
    
    operator VkPipeline() const { return PipelineHandle; }
    operator const VkPipeline*() const { return &PipelineHandle; }
    
    bool IsValid() const { return PipelineLayoutHandle != VK_NULL_HANDLE && PipelineHandle != VK_NULL_HANDLE; }
    
    void Initialize(const VkPipelineLayoutCreateInfo& InPipelineLayoutInfo, VkGraphicsPipelineCreateInfo& InGraphicsPipelineInfo)
    {
        check(vkCreatePipelineLayout(Device, &InPipelineLayoutInfo, nullptr, &PipelineLayoutHandle) == VK_SUCCESS, "FErolyssaPipeline: Failed to create graphics pipeline layout!");
        
        InGraphicsPipelineInfo.layout = PipelineLayoutHandle;
        check(vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &InGraphicsPipelineInfo, nullptr, &PipelineHandle) == VK_SUCCESS, "FErolyssaPipeline: Failed to create graphics pipeline!");
    }
    
    void Initialize(const VkPipelineLayoutCreateInfo& InPipelineLayoutInfo, VkComputePipelineCreateInfo& InComputePipelineInfo)
    {
        check(vkCreatePipelineLayout(Device, &InPipelineLayoutInfo, nullptr, &PipelineLayoutHandle) == VK_SUCCESS, "FErolyssaPipeline: Failed to create compute pipeline layout");
        
        InComputePipelineInfo.layout = PipelineLayoutHandle;
        check(vkCreateComputePipelines(Device, VK_NULL_HANDLE, 1, &InComputePipelineInfo, nullptr, &PipelineHandle) == VK_SUCCESS, "FErolyssaPipeline: Failed to create compute pipeline!");
    }

private:
    const FErolyssaDevice& Device;
    
    VkPipelineLayout PipelineLayoutHandle = VK_NULL_HANDLE;
    VkPipeline PipelineHandle = VK_NULL_HANDLE;
};
