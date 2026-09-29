// Root Engine / Erolyssa

#pragma once

#include <vulkan/vulkan.h>

class FErolyssaPhysicalDevice;

class FErolyssaBuffer
{
public:
    FErolyssaBuffer(const FErolyssaPhysicalDevice& InDevice, VkDeviceSize InSize, VkBufferUsageFlags InUsage, VkMemoryPropertyFlags InProperties);
    ~FErolyssaBuffer();
    
    void* Map();
    void Unmap();
    bool CopyFrom(const void* InData, const int32_t InSize);
    
    VkBuffer GetHandle() const { return BufferHandle; }
    VkDeviceMemory GetMemoryHandle() const { return MemoryHandle; }
    VkDeviceSize GetSize() const { return Size; }

private:
    VkDevice LogicalDeviceRef = VK_NULL_HANDLE;
    
    VkBuffer BufferHandle = VK_NULL_HANDLE;
    VkDeviceMemory MemoryHandle = VK_NULL_HANDLE;
    VkDeviceSize Size = 0;
    void* MappedPointer = nullptr;

    static uint32_t FindMemoryType(VkPhysicalDevice PhysicalDevice, uint32_t TypeFilter, VkMemoryPropertyFlags Properties);
};
