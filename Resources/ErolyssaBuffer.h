// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Erolyssa.h"
#include "../Core/ErolyssaDevice.h"

enum class EErolyssaBufferUsage : uint8_t
{
    None          = 0,
    Vertex        = 1 << 0,
    Index         = 1 << 1,
    Uniform       = 1 << 2,
    Storage       = 1 << 3,
    Indirect      = 1 << 4,
    TransferSrc   = 1 << 5,
    TransferDst   = 1 << 6,
    DeviceAddress = 1 << 7,
};
ENUM_CLASS_FLAGS(EErolyssaBufferUsage)

enum class EErolyssaMemoryProperty : uint8_t
{
    None            = 0,
    DeviceLocal     = 1 << 0,  // быстрая для GPU, недоступна CPU
    HostVisible     = 1 << 1,  // доступна CPU
    HostCoherent    = 1 << 2,  // без flush/invalidate
    HostCached      = 1 << 3,  // кэшируемая (медленнее, но читается быстро)
    LazilyAllocated = 1 << 4,
};
ENUM_CLASS_FLAGS(EErolyssaMemoryProperty)

class FErolyssaBuffer
{
    
public:
    FErolyssaBuffer(
        const FErolyssaDevice& InDevice, const FErolyssaSize InSize,
        const EErolyssaBufferUsage InUsage, const EErolyssaMemoryProperty InMemoryProperties
    ) : Device(InDevice), Size(InSize), MemoryProperties(InMemoryProperties)
    {
        check(InSize > 0, "FErolyssaBuffer: size must be > 0!");
        
        VkBufferCreateInfo BufferInfo{};
        BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferInfo.size = InSize;
        BufferInfo.usage = ToVulkan(InUsage);
        BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        check(vkCreateBuffer(Device, &BufferInfo, nullptr, &Handle) == VK_SUCCESS, "FErolyssaBuffer: failed to create buffer!");
        
        VkMemoryRequirements MemReq{};
        vkGetBufferMemoryRequirements(Device, Handle, &MemReq);
        
        VkMemoryAllocateFlagsInfo AllocFlagsInfo{};
        AllocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        AllocFlagsInfo.pNext = nullptr;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::DeviceAddress)) AllocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
        
        VkMemoryAllocateInfo AllocInfo{};
        AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        AllocInfo.pNext = &AllocFlagsInfo;
        AllocInfo.allocationSize = MemReq.size;
        AllocInfo.memoryTypeIndex = FindMemoryType(Device, MemReq.memoryTypeBits, ToVulkan(InMemoryProperties));
        
        check(vkAllocateMemory(Device, &AllocInfo, nullptr, &Memory) == VK_SUCCESS, "FErolyssaBuffer: failed to allocate memory!");
        check(vkBindBufferMemory(Device, Handle, Memory, 0) == VK_SUCCESS, "FErolyssaBuffer: failed to bind memory!");
    }
    
    ~FErolyssaBuffer()
    {
        if(MappedPointer != nullptr) Unmap();
        if(Handle != VK_NULL_HANDLE) vkDestroyBuffer(Device, Handle, nullptr);
        if(Memory != VK_NULL_HANDLE) vkFreeMemory(Device, Memory, nullptr);
    }
    
    FErolyssaBuffer(const FErolyssaBuffer&) = delete;
    FErolyssaBuffer& operator=(const FErolyssaBuffer&) = delete;
    
    operator VkBuffer() const { return Handle; }
    operator const VkBuffer*() const { return &Handle; }
    bool IsValid() const { return Handle != VK_NULL_HANDLE; }
    
    VkDeviceMemory GetMemory() const { return Memory; }
    FErolyssaSize GetSize() const { return Size; }
    
    void Upload(const void* InData, const FErolyssaSize InSize, const FErolyssaSize InOffset = 0) const
    {
        check(InSize > 0, "FErolyssaBuffer::Upload: size must be > 0!");
        check(InOffset + InSize <= Size, "FErolyssaBuffer::Upload: out of bounds!");
        check(IsHostVisible(), "FErolyssaBuffer::Upload: buffer is not host-visible!");
        
        void* Data = nullptr;
        check(vkMapMemory(Device, Memory, InOffset, InSize, 0, &Data) == VK_SUCCESS, "FErolyssaBuffer::Upload: failed to map memory!");
        memcpy(Data, InData, InSize);
        
        vkUnmapMemory(Device, Memory);
    }
    
    void* Map(const FErolyssaSize InOffset = 0, const FErolyssaSize InSize = VK_WHOLE_SIZE)
    {
        check(IsHostVisible(), "FErolyssaBuffer::Map: buffer is not host-visible!");
        check(MappedPointer == nullptr, "FErolyssaBuffer::Map: already mapped!");
        check(vkMapMemory(Device, Memory, InOffset, InSize, 0, &MappedPointer) == VK_SUCCESS, "FErolyssaBuffer::Map: failed to map memory!");
        
        return MappedPointer;
    }
    
    void Unmap()
    {
        if(MappedPointer != nullptr)
        {
            vkUnmapMemory(Device, Memory);
            MappedPointer = nullptr;
        }
    }
    
    FErolyssaAddress GetAddress() const
    {
        VkBufferDeviceAddressInfo Info{};
        Info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        Info.buffer = Handle;
        return vkGetBufferDeviceAddress(Device, &Info);
    }
    
    bool IsHostVisible() const { return EnumHasAnyFlags(MemoryProperties, EErolyssaMemoryProperty::HostVisible); }
    bool IsDeviceLocal() const { return EnumHasAnyFlags(MemoryProperties, EErolyssaMemoryProperty::DeviceLocal); }

private:
    const FErolyssaDevice& Device;
    
    VkBuffer Handle = VK_NULL_HANDLE;
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    FErolyssaSize Size = 0;
    EErolyssaMemoryProperty MemoryProperties = EErolyssaMemoryProperty::None;
    
    void* MappedPointer = nullptr;
    
    static VkBufferUsageFlags ToVulkan(const EErolyssaBufferUsage InUsage)
    {
        VkBufferUsageFlags Result = 0;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::Vertex       )) Result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::Index        )) Result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::Uniform      )) Result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::Storage      )) Result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::Indirect     )) Result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::TransferSrc  )) Result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::TransferDst  )) Result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if(EnumHasAnyFlags(InUsage, EErolyssaBufferUsage::DeviceAddress)) Result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        return Result;
    }
    
    static VkMemoryPropertyFlags ToVulkan(const EErolyssaMemoryProperty InProps)
    {
        VkMemoryPropertyFlags Result = 0;
        if(EnumHasAnyFlags(InProps, EErolyssaMemoryProperty::DeviceLocal    )) Result |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        if(EnumHasAnyFlags(InProps, EErolyssaMemoryProperty::HostVisible    )) Result |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        if(EnumHasAnyFlags(InProps, EErolyssaMemoryProperty::HostCoherent   )) Result |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        if(EnumHasAnyFlags(InProps, EErolyssaMemoryProperty::HostCached     )) Result |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        if(EnumHasAnyFlags(InProps, EErolyssaMemoryProperty::LazilyAllocated)) Result |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
        return Result;
    }
    
    static uint32_t FindMemoryType(const VkPhysicalDevice InPhysical, const uint32_t InTypeFilter, const VkMemoryPropertyFlags InProperties)
    {
        VkPhysicalDeviceMemoryProperties MemProps{};
        vkGetPhysicalDeviceMemoryProperties(InPhysical, &MemProps);
        
        for(uint32_t i = 0; i < MemProps.memoryTypeCount; ++i)
            if((InTypeFilter & (1u << i)) && (MemProps.memoryTypes[i].propertyFlags & InProperties) == InProperties)
                return i;
        
        FErolyssaDebug::Terminate("FErolyssaBuffer: no suitable memory type found!");
        return 0xFFFFFFFF;
    }
};
