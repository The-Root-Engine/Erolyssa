// Root Engine / Erolyssa

#pragma once

#include "ErolyssaDevice.h"
#include "ErolyssaSurface.h"

#include <SDL_video.h>
#include <algorithm>

class FErolyssaSwapchain
{
    
public:
    FErolyssaSwapchain(
        const FErolyssaDevice& InDevice,
        const FErolyssaSurface& InSurface,
        uint32_t InWidth,
        uint32_t InHeight
    ) : Device(InDevice)
    {
        // Device хранит ссылку на PhysicalDevice
        VkSurfaceKHR Surface = InSurface;  // неявное преобразование через operator VkSurfaceKHR()
        
        // 1. Capabilities
        VkSurfaceCapabilitiesKHR Caps{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Caps);
        
        // 2. Formats
        uint32_t FormatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr);
        check(FormatCount > 0, "FErolyssaSwapchain: no surface formats!");
        std::vector<VkSurfaceFormatKHR> Formats(FormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Formats.data());
        
        // 3. Present modes
        uint32_t ModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &ModeCount, nullptr);
        check(ModeCount > 0, "FErolyssaSwapchain: no present modes!");
        std::vector<VkPresentModeKHR> Modes(ModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &ModeCount, Modes.data());
        
        // 4. Выбор параметров
        VkSurfaceFormatKHR SurfaceFormat = ChooseSurfaceFormat(Formats);
        VkPresentModeKHR PresentMode = ChoosePresentMode(Modes);
        Extent = ChooseExtent(Caps, InWidth, InHeight);
        
        ImageFormat = SurfaceFormat.format;
        
        // 5. Количество images
        uint32_t ImageCount = Caps.minImageCount + 1;
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
        uint32_t GraphicsFamily = Device.GetGraphicsFamily();
        uint32_t PresentFamily = Device.GetPresentFamily();
        uint32_t FamilyIndices[] = { GraphicsFamily, PresentFamily };
        
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
        uint32_t ActualCount = 0;
        vkGetSwapchainImagesKHR(Device, Handle, &ActualCount, nullptr);
        Images.resize(ActualCount);
        vkGetSwapchainImagesKHR(Device, Handle, &ActualCount, Images.data());
        
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
    uint32_t GetImageCount() const { return static_cast<uint32_t>(Images.size()); }
    
    VkImage GetImage(const uint32_t InIndex) const { return Images[InIndex]; }
    VkImageView GetImageView(const uint32_t InIndex) const { return ImageViews[InIndex]; }

private:
    const FErolyssaDevice& Device;
    VkSwapchainKHR Handle = VK_NULL_HANDLE;
    
    VkFormat ImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D Extent{};
    
    std::vector<VkImage> Images;
    std::vector<VkImageView> ImageViews;

    static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InFormats)
    {
        for(const auto& F : InFormats)
            if(F.format == VK_FORMAT_B8G8R8A8_SRGB && F.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return F;
        
        return InFormats[0];
    }
    
    static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& InModes)
    {
        for(const VkPresentModeKHR Mode : InModes)
            if(Mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return Mode;
        
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& InCaps, const uint32_t InWidth, const uint32_t InHeight)
    {
        if(InCaps.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return InCaps.currentExtent;
        
        VkExtent2D ResultExtent{};
        ResultExtent.width  = std::clamp(InWidth,  InCaps.minImageExtent.width,  InCaps.maxImageExtent.width);
        ResultExtent.height = std::clamp(InHeight, InCaps.minImageExtent.height, InCaps.maxImageExtent.height);
        return ResultExtent;
    }
    
    void CreateImageViews()
    {
        ImageViews.resize(Images.size());
        
        for(size_t i = 0; i < Images.size(); ++i)
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
