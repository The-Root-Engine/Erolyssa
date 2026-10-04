// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Erolyssa.h"
#include "../Core/ErolyssaDevice.h"

class FErolyssaImage
{
    
public:
    FErolyssaImage(const FErolyssaDevice& InDevice)
        : Device(InDevice) {}
    
    FErolyssaImage(const FErolyssaDevice& InDevice, const VkImageCreateInfo& InImageInfo, VkImageViewCreateInfo& InImageViewInfo)
        : Device(InDevice) { Initialize(InImageInfo, InImageViewInfo); }
    
    ~FErolyssaImage()
    {
        if(ImageViewHandle) vkDestroyImageView(Device, ImageViewHandle, nullptr);
        if(ImageHandle) vkDestroyImage(Device, ImageHandle, nullptr);
        if(DeviceMemoryHandle) vkFreeMemory(Device, DeviceMemoryHandle, nullptr);
    }
    
    FErolyssaImage(const FErolyssaImage&) = delete;
    FErolyssaImage& operator=(const FErolyssaImage&) = delete;
    
    operator VkImage() const { return ImageHandle; }
    operator const VkImage*() const { return &ImageHandle; }
    
    operator VkImageView() const { return ImageViewHandle; }
    operator const VkImageView*() const { return &ImageViewHandle; }
    
    operator VkDeviceMemory() const { return DeviceMemoryHandle; }
    operator const VkDeviceMemory*() const { return &DeviceMemoryHandle; }
    
    /*
    bool IsValid() const { return BufferHandle != VK_NULL_HANDLE; }
    */
    
    VkExtent2D GetExtent() const { return ImageExtent; }
    
    void Initialize(const VkImageCreateInfo& InImageInfo, VkImageViewCreateInfo& InImageViewInfo)
    {
        ImageExtent = VkExtent2D{ InImageInfo.extent.width, InImageInfo.extent.height };
        
        check(vkCreateImage(Device, &InImageInfo, nullptr, &ImageHandle) == VK_SUCCESS, "FErolyssaImage: Failed to create Image!");
        
        VkMemoryRequirements MemoryRequirements;
        vkGetImageMemoryRequirements(Device, ImageHandle, &MemoryRequirements);
        
        VkMemoryAllocateInfo MemoryAllocateInfo{};
        MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
        MemoryAllocateInfo.memoryTypeIndex = ErolyssaFindMemoryType(Device, MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        check(vkAllocateMemory(Device, &MemoryAllocateInfo, nullptr, &DeviceMemoryHandle) == VK_SUCCESS, "FErolyssaImage: Failed to allocate Device Memory!");
        
        vkBindImageMemory(Device, ImageHandle, DeviceMemoryHandle, 0);
        
        InImageViewInfo.image = ImageHandle;
        check(vkCreateImageView(Device, &InImageViewInfo, nullptr, &ImageViewHandle) == VK_SUCCESS, "FErolyssaImage: Failed to create Image View!");
    }

private:
    const FErolyssaDevice& Device;
    
    VkExtent2D ImageExtent{};
    
    VkImage ImageHandle = VK_NULL_HANDLE;
    VkImageView ImageViewHandle = VK_NULL_HANDLE;
    VkDeviceMemory DeviceMemoryHandle = VK_NULL_HANDLE;
};
