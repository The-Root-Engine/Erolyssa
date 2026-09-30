// Root Engine / Erolyssa

#pragma once

#include <vulkan/vulkan.h>

class FErolyssaDevice;

class FErolyssaImageView
{
public:
    FErolyssaImageView(const FErolyssaDevice& InDevice, FErolyssaSize InSize, VkBufferUsageFlags InUsage, VkMemoryPropertyFlags InProperties);
    ~FErolyssaImageView();
    
    void* Map();
    void Unmap();
    bool CopyFrom(const void* InData, const int32_t InSize);
    
    VkBuffer GetHandle() const { return BufferHandle; }
    VkDeviceMemory GetMemoryHandle() const { return MemoryHandle; }
    FErolyssaSize GetSize() const { return Size; }

private:
    VkDevice LogicalDeviceRef = VK_NULL_HANDLE;
    
    VkBuffer BufferHandle = VK_NULL_HANDLE;
    VkDeviceMemory MemoryHandle = VK_NULL_HANDLE;
    FErolyssaSize Size = 0;
    void* MappedPointer = nullptr;
};
