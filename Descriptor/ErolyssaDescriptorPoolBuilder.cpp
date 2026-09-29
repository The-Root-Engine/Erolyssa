#include "ErolyssaDescriptorPoolBuilder.h"

#include <vulkan/vulkan_core.h>

#include "Erolyssa.h"
#include "ErolyssaDescriptorPool.h"
#include "ErolyssaDescriptorSet.h"
#include "ErolyssaDescriptorSetBuilder.h"
#include "ErolyssaPhysicalDevice.h"
#include "ErolyssaPipeline.h"

FErolyssaDescriptorPoolBuilder::FErolyssaDescriptorPoolBuilder(FErolyssaPhysicalDevice& InDevice, FErolyssaDescriptorPool& OutPool)
    : Device(InDevice), Pool(OutPool)
{
    SetBuilders.reserve(8);
}

FErolyssaDescriptorPoolBuilder& FErolyssaDescriptorPoolBuilder::BindSet(FErolyssaDescriptorSetBuilder SetBuilder)
{
    SetBuilders.push_back(std::move(SetBuilder));
    return *this;
}

void FErolyssaDescriptorPoolBuilder::Build()
{
    std::vector<VkDescriptorPoolSize> CombinedSizes;
    CombinedSizes.reserve(SetBuilders.size());
    
    for(const auto& SetBuilder : SetBuilders)
    {
        for(const auto& Size : SetBuilder.PoolSizes)
        {
            bool bFound = false;
            for(auto& CombinedSize : CombinedSizes) 
            {
                if(CombinedSize.type == Size.type) 
                {
                    CombinedSize.descriptorCount += Size.descriptorCount;
                    bFound = true;
                    break;
                }
            }
            
            if(!bFound) CombinedSizes.push_back(Size);
        }
    }
    
    VkDescriptorPoolCreateInfo PoolInfo{};
    PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    PoolInfo.flags = 0;
    PoolInfo.maxSets = static_cast<uint32_t>(SetBuilders.size());
    PoolInfo.poolSizeCount = static_cast<uint32_t>(CombinedSizes.size());
    PoolInfo.pPoolSizes = CombinedSizes.data();
    
    if(vkCreateDescriptorPool(Device.GetLogicalHandle(), &PoolInfo, nullptr, &Pool.Handle) != VK_SUCCESS)
        ErolyssaTerminate("Failed to create unified descriptor pool!");
    
    for(auto& SetBuilder : SetBuilders)
    {
        VkDescriptorSetAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        AllocInfo.descriptorPool = Pool.Handle;
        AllocInfo.descriptorSetCount = 1;
        AllocInfo.pSetLayouts = &SetBuilder.Pipeline.DescriptorSetLayoutHandle;
        
        if(vkAllocateDescriptorSets(Device.GetLogicalHandle(), &AllocInfo, &SetBuilder.Set.Handle) != VK_SUCCESS)
            ErolyssaTerminate("Failed to allocate descriptor set from unified pool!");
        
        for(size_t i = 0; i < SetBuilder.Writes.size(); ++i)
        {
            SetBuilder.Writes[i].dstSet = SetBuilder.Set.Handle;
            SetBuilder.Writes[i].pBufferInfo = &SetBuilder.BufferInfos[i];
        }
        
        vkUpdateDescriptorSets(
            Device.GetLogicalHandle(), 
            static_cast<uint32_t>(SetBuilder.Writes.size()), 
            SetBuilder.Writes.data(), 
            0, nullptr
        );
    }
}
