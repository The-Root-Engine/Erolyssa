// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../ErolyssaAliases.h"
#include "ErolyssaDevice.h"
#include "ErolyssaSurface.h"

#include <SDL_video.h>

class FErolyssaSwapchain
{
    
public:
    FErolyssaSwapchain(
        const FErolyssaDevice& InDevice,
        const FErolyssaSurface& InSurface,
        uint32 InWidth,
        uint32 InHeight
    ) : Device(InDevice)
    {
        // Device хранит ссылку на PhysicalDevice
        VkSurfaceKHR Surface = InSurface;  // неявное преобразование через operator VkSurfaceKHR()
        
        // 1. Capabilities
        VkSurfaceCapabilitiesKHR Caps{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Caps);
        
        // 2. Formats
        uint32 FormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr);
        check(FormatCount > 0, "FErolyssaSwapchain: no surface formats!");
        TArray<VkSurfaceFormatKHR> Formats(FormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Formats.Data());
        
        // 3. Present modes
        uint32 ModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &ModeCount, nullptr);
        check(ModeCount > 0, "FErolyssaSwapchain: no present modes!");
        TArray<VkPresentModeKHR> Modes(ModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &ModeCount, Modes.Data());
        
        // 4. Выбор параметров
        VkSurfaceFormatKHR SurfaceFormat = ChooseSurfaceFormat(Formats);
        VkPresentModeKHR PresentMode = ChoosePresentMode(Modes);
        Extent = ChooseExtent(Caps, InWidth, InHeight);
        
        ImageFormat = SurfaceFormat.format;
        
        // 5. Количество images
        uint32 ImageCount = Caps.minImageCount + 1;
        if(Caps.maxImageCount > 0 && ImageCount > Caps.maxImageCount)
            ImageCount = Caps.maxImageCount;
        
        // 6. Create info
        VkSwapchainCreateInfoKHR CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        CreateInfo.surface = Surface;
        CreateInfo.minImageCount = ImageCount;
        CreateInfo.imageFormat = SurfaceFormat.format;
        CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
        CreateInfo.imageExtent = Extent;
        CreateInfo.imageArrayLayers = 1;
        CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        
        // 7. Sharing mode
        uint32 GraphicsFamily = Device.GetGraphicsFamily();
        uint32 PresentFamily = Device.GetPresentFamily();
        uint32 FamilyIndices[] = { GraphicsFamily, PresentFamily };
        
        if(GraphicsFamily != PresentFamily)
        {
            CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            CreateInfo.queueFamilyIndexCount = 2;
            CreateInfo.pQueueFamilyIndices = FamilyIndices;
        }
        else
        {
            CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        
        CreateInfo.preTransform = Caps.currentTransform;
        CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        CreateInfo.presentMode = PresentMode;
        CreateInfo.clipped = VK_TRUE;
        CreateInfo.oldSwapchain = VK_NULL_HANDLE;
        
        check(vkCreateSwapchainKHR(Device, &CreateInfo, nullptr, &Handle) == VK_SUCCESS, "FErolyssaSwapchain: failed to create swapchain!");
        
        // 8. Получаем images
        uint32 ActualCount = 0;
        vkGetSwapchainImagesKHR(Device, Handle, &ActualCount, nullptr);
        Images.SetNum(ActualCount);
        vkGetSwapchainImagesKHR(Device, Handle, &ActualCount, Images.Data());
        
        // 9. Создаём views
        CreateImageViews();
    }
    
    ~FErolyssaSwapchain()
    {
        for(const VkImageView View : ImageViews) vkDestroyImageView(Device, View, nullptr);
        if(Handle != VK_NULL_HANDLE) vkDestroySwapchainKHR(Device, Handle, nullptr);
    }
    
    FErolyssaSwapchain(const FErolyssaSwapchain&) = delete;
    FErolyssaSwapchain& operator=(const FErolyssaSwapchain&) = delete;
    
    operator VkSwapchainKHR() const { return Handle; }
    operator const VkSwapchainKHR*() const { return &Handle; }
    
    VkFormat GetImageFormat() const { return ImageFormat; }
    VkExtent2D GetExtent() const { return Extent; }
    uint32 GetImageCount() const { return static_cast<uint32>(Images.Num()); }
    
    VkImage GetImage(const uint32 InIndex) const { return Images[InIndex]; }
    VkImageView GetImageView(const uint32 InIndex) const { return ImageViews[InIndex]; }

private:
    const FErolyssaDevice& Device;
    VkSwapchainKHR Handle = VK_NULL_HANDLE;
    
    VkFormat ImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D Extent{};
    
    TArray<VkImage> Images;
    TArray<VkImageView> ImageViews;
    
    static VkSurfaceFormatKHR ChooseSurfaceFormat(const TArray<VkSurfaceFormatKHR>& InFormats)
    {
        for(const auto& F : InFormats)
            if(F.format == VK_FORMAT_B8G8R8A8_SRGB && F.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return F;
        
        return InFormats[0];
    }
    
    static VkPresentModeKHR ChoosePresentMode(const TArray<VkPresentModeKHR>& InModes)
    {
        for(const VkPresentModeKHR Mode : InModes)
            if(Mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return Mode;
        
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& InCaps, const uint32 InWidth, const uint32 InHeight)
    {
        if(InCaps.currentExtent.width != UINT32_MAX)
            return InCaps.currentExtent;
        
        VkExtent2D ResultExtent{};
        ResultExtent.width  = FMath::Clamp(InWidth,  InCaps.minImageExtent.width,  InCaps.maxImageExtent.width);
        ResultExtent.height = FMath::Clamp(InHeight, InCaps.minImageExtent.height, InCaps.maxImageExtent.height);
        return ResultExtent;
    }
    
    void CreateImageViews()
    {
        ImageViews.SetNum(Images.Num());
        for(size_t i = 0; i < Images.Num(); ++i)
        {
            VkImageViewCreateInfo CreateInfo{};
            CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            CreateInfo.image = Images[i];
            CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            CreateInfo.format = ImageFormat;
            CreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            CreateInfo.subresourceRange.baseMipLevel = 0;
            CreateInfo.subresourceRange.levelCount = 1;
            CreateInfo.subresourceRange.baseArrayLayer = 0;
            CreateInfo.subresourceRange.layerCount = 1;
            
            check(vkCreateImageView(Device, &CreateInfo, nullptr, &ImageViews[i]) == VK_SUCCESS, "FErolyssaSwapchain: failed to create image view %zu", i);
        }
    }
};
