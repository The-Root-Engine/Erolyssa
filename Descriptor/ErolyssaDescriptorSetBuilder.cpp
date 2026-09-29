#include "ErolyssaDescriptorSetBuilder.h"

#include <vulkan/vulkan_core.h>

#include "ErolyssaPhysicalDevice.h"


FErolyssaDescriptorSetBuilder::FErolyssaDescriptorSetBuilder(FErolyssaPipeline& InPipeline, FErolyssaDescriptorSet& OutSet)
    : Pipeline(InPipeline), Set(OutSet)
{
    Writes.reserve(8);
    BufferInfos.reserve(8);
    PoolSizes.reserve(8);
}

FErolyssaDescriptorSetBuilder& FErolyssaDescriptorSetBuilder::BindBuffer(const uint32_t Binding, const FErolyssaBuffer& Buffer, const VkDescriptorType Type)
{
    VkDescriptorBufferInfo Info{};
    Info.buffer = Buffer.GetHandle();
    Info.offset = 0;
    Info.range  = Buffer.GetSize();
    BufferInfos.push_back(Info);
    
    VkWriteDescriptorSet Write{};
    Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Write.dstBinding = Binding;
    Write.dstArrayElement = 0;
    Write.descriptorType = Type;
    Write.descriptorCount = 1;
    Writes.push_back(Write);
    
    VkDescriptorPoolSize Size{};
    Size.type = Type;
    Size.descriptorCount = 1;
    PoolSizes.push_back(Size);
        
    return *this;
}
