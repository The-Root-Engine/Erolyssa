// Root Engine / Erolyssa

#pragma once

#include "ErolyssaDescriptorPool.h"
#include "FErolyssaDescriptorSetLayout.h"
#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Core/ErolyssaDevice.h"
#include "../Resources/ErolyssaBuffer.h"
#include "../Resources/FErolyssaSampler.h"

class FErolyssaDescriptorSet
{
    
public:
    FErolyssaDescriptorSet(
        const FErolyssaDevice& InDevice,
        const FErolyssaDescriptorPool& InPool,
        const FErolyssaDescriptorSetLayout& InLayout
    ) : Device(InDevice)
    {
        VkDescriptorSetAllocateInfo Info{};
        Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        Info.descriptorPool = InPool;
        Info.descriptorSetCount = 1;
        Info.pSetLayouts = InLayout;
        
        check(vkAllocateDescriptorSets(Device, &Info, &Handle) == VK_SUCCESS,
              "FErolyssaDescriptorSet: failed to allocate!");
    }
    
    ~FErolyssaDescriptorSet() = default;
    
    FErolyssaDescriptorSet(const FErolyssaDescriptorSet&) = delete;
    FErolyssaDescriptorSet& operator=(const FErolyssaDescriptorSet&) = delete;
    
    operator VkDescriptorSet() const { return Handle; }
    operator const VkDescriptorSet*() const { return &Handle; }
    bool IsValid() const { return Handle != VK_NULL_HANDLE; }
    
    void WriteStorageBuffer(uint32 InBinding, const FErolyssaBuffer& InBuffer) const
    {
        VkDescriptorBufferInfo BufInfo{};
        BufInfo.buffer = InBuffer;
        BufInfo.offset = 0;
        BufInfo.range  = VK_WHOLE_SIZE;
        
        VkWriteDescriptorSet Write{};
        Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        Write.dstSet = Handle;
        Write.dstBinding = InBinding;
        Write.descriptorCount = 1;
        Write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        Write.pBufferInfo = &BufInfo;
        
        vkUpdateDescriptorSets(Device, 1, &Write, 0, nullptr);
    }
    
    void WriteUniformBuffer(uint32 InBinding, const FErolyssaBuffer& InBuffer) const
    {
        VkDescriptorBufferInfo BufInfo{};
        BufInfo.buffer = InBuffer;
        BufInfo.offset = 0;
        BufInfo.range  = VK_WHOLE_SIZE;
        
        VkWriteDescriptorSet Write{};
        Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        Write.dstSet = Handle;
        Write.dstBinding = InBinding;
        Write.descriptorCount = 1;
        Write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        Write.pBufferInfo = &BufInfo;
        
        vkUpdateDescriptorSets(Device, 1, &Write, 0, nullptr);
    }
    
    void WriteCombinedImageSampler(uint32 InBinding, VkImageView InView, const FErolyssaSampler& InSampler) const
    {
        VkDescriptorImageInfo ImageInfo{};
        ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        ImageInfo.imageView = InView;
        ImageInfo.sampler = InSampler;
        
        VkWriteDescriptorSet Write{};
        Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        Write.dstSet = Handle;
        Write.dstBinding = InBinding;
        Write.descriptorCount = 1;
        Write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        Write.pImageInfo = &ImageInfo;
        
        vkUpdateDescriptorSets(Device, 1, &Write, 0, nullptr);
    }

private:
    const FErolyssaDevice& Device;
    VkDescriptorSet Handle = VK_NULL_HANDLE;
};
