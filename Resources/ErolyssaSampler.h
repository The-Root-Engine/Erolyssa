// Root Engine / Erolyssa

#pragma once

#include <vulkan/vulkan.h>

class FErolyssaPhysicalDevice;

class FErolyssaSampler
{
    
public:
    FErolyssaSampler(const FErolyssaPhysicalDevice& InDevice, FErolyssaSize InSize, VkBufferUsageFlags InUsage, VkMemoryPropertyFlags InProperties);
    ~FErolyssaSampler();
    
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

    static uint32_t FindMemoryType(VkPhysicalDevice PhysicalDevice, uint32_t TypeFilter, VkMemoryPropertyFlags Properties);
};
