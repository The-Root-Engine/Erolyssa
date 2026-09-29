#include "ErolyssaDescriptorSet.h"

#include "ErolyssaBuffer.h"
#include "ErolyssaDescriptorPool.h"
#include "ErolyssaPhysicalDevice.h"

/*
FErolyssaDescriptorSet& FErolyssaDescriptorSet::BindBuffer(uint32_t Binding, const FErolyssaBuffer& Buffer, VkDescriptorType Type)
{
    VkDescriptorBufferInfo& Info = BufferInfos.emplace_back();
    Info.buffer = Buffer.GetHandle();
    Info.offset = 0;
    Info.range  = Buffer.GetSize();

    VkWriteDescriptorSet Write{};
    Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Write.dstBinding = Binding;
    Write.descriptorType = Type;
    Write.descriptorCount = 1;
    Writes.push_back(Write);

    return *this;
}

VkDescriptorSet FErolyssaDescriptorSet::Build(VkDescriptorSetLayout Layout)
{
    VkDescriptorSet Set = Pool.Allocate(Layout);
    
    for (size_t i = 0; i < Writes.size(); ++i) {
        Writes[i].dstSet = Set;
        Writes[i].pBufferInfo = &BufferInfos[i];
    }
    
    vkUpdateDescriptorSets(Device.GetLogicalHandle(), static_cast<uint32_t>(Writes.size()), Writes.data(), 0, nullptr);
    return Set;
}
*/
