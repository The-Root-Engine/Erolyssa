// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../ErolyssaAliases.h"

class FErolyssaPhysicalDevice;

class FErolyssaSampler
{
    
public:
    FErolyssaSampler(const FErolyssaPhysicalDevice& InDevice, FErolyssaSize InSize, VkBufferUsageFlags InUsage, VkMemoryPropertyFlags InProperties);
    ~FErolyssaSampler();
    
    void* Map();
    void Unmap();
    bool CopyFrom(const void* InData, const int32 InSize);
    
    VkBuffer GetHandle() const { return BufferHandle; }
    VkDeviceMemory GetMemoryHandle() const { return MemoryHandle; }
    FErolyssaSize GetSize() const { return Size; }

private:
    VkDevice LogicalDeviceRef = VK_NULL_HANDLE;
    
    VkBuffer BufferHandle = VK_NULL_HANDLE;
    VkDeviceMemory MemoryHandle = VK_NULL_HANDLE;
    FErolyssaSize Size = 0;
    void* MappedPointer = nullptr;

    static uint32 FindMemoryType(VkPhysicalDevice PhysicalDevice, uint32 TypeFilter, VkMemoryPropertyFlags Properties);
};
