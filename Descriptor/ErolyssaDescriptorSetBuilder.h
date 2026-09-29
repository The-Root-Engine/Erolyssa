#pragma once

#include <vector>
#include "ErolyssaBuffer.h"

class FErolyssaPipeline;
class FErolyssaDescriptorSet;
struct FErolyssaDescriptorPool;
class FErolyssaPhysicalDevice;

struct FErolyssaDescriptorSetBuilder
{
    friend struct FErolyssaDescriptorPoolBuilder;
    
public:
    FErolyssaDescriptorSetBuilder(FErolyssaPipeline& InPipeline, FErolyssaDescriptorSet& OutSet);
    
    FErolyssaDescriptorSetBuilder& BindBuffer(uint32_t Binding, const FErolyssaBuffer& Buffer, VkDescriptorType Type);

private:
    FErolyssaPipeline& Pipeline;
    FErolyssaDescriptorSet& Set;
    
    std::vector<VkWriteDescriptorSet> Writes;
    std::vector<VkDescriptorBufferInfo> BufferInfos;
    std::vector<VkDescriptorPoolSize> PoolSizes;
};
