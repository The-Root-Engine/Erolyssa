// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Core/ErolyssaDevice.h"

enum class EErolyssaFilter : uint8_t
{
    Nearest = 0,
    Linear  = 1,
};

inline VkFilter ToVulkan(EErolyssaFilter InFilter)
{
    switch(InFilter)
    {
    case EErolyssaFilter::Nearest: return VK_FILTER_NEAREST;
    case EErolyssaFilter::Linear:  return VK_FILTER_LINEAR;
    }
    return VK_FILTER_NEAREST;
}

enum class EErolyssaSamplerAddressMode : uint8_t
{
    Repeat             = 0,
    MirroredRepeat     = 1,
    ClampToEdge        = 2,
    ClampToBorder      = 3,
    MirrorClampToEdge  = 4,
};

inline VkSamplerAddressMode ToVulkan(EErolyssaSamplerAddressMode InMode)
{
    switch(InMode)
    {
    case EErolyssaSamplerAddressMode::Repeat:            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case EErolyssaSamplerAddressMode::MirroredRepeat:    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case EErolyssaSamplerAddressMode::ClampToEdge:       return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case EErolyssaSamplerAddressMode::ClampToBorder:     return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case EErolyssaSamplerAddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    }
    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
}

enum class EErolyssaBorderColor : uint8_t
{
    FloatTransparentBlack = 0,
    IntTransparentBlack   = 1,
    FloatOpaqueBlack      = 2,
    IntOpaqueBlack        = 3,
    FloatOpaqueWhite      = 4,
    IntOpaqueWhite        = 5,
};

inline VkBorderColor ToVulkan(EErolyssaBorderColor InColor)
{
    switch(InColor)
    {
    case EErolyssaBorderColor::FloatTransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    case EErolyssaBorderColor::IntTransparentBlack:   return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    case EErolyssaBorderColor::FloatOpaqueBlack:      return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    case EErolyssaBorderColor::IntOpaqueBlack:        return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    case EErolyssaBorderColor::FloatOpaqueWhite:      return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    case EErolyssaBorderColor::IntOpaqueWhite:        return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
    }
    return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
}

class FErolyssaSampler
{
    
public:
    FErolyssaSampler(
        const FErolyssaDevice& InDevice,
        const EErolyssaFilter InMagFilter = EErolyssaFilter::Nearest,
        const EErolyssaFilter InMinFilter = EErolyssaFilter::Nearest,
        const EErolyssaSamplerAddressMode InAddressMode = EErolyssaSamplerAddressMode::ClampToEdge,
        const EErolyssaBorderColor InBorderColor = EErolyssaBorderColor::IntOpaqueBlack,
        const bool bUnnormalized = false
    ) : Device(InDevice)
    {
        VkSamplerCreateInfo Info{};
        Info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        Info.magFilter = ToVulkan(InMagFilter);
        Info.minFilter = ToVulkan(InMinFilter);
        Info.addressModeU = ToVulkan(InAddressMode);
        Info.addressModeV = ToVulkan(InAddressMode);
        Info.addressModeW = ToVulkan(InAddressMode);
        Info.borderColor = ToVulkan(InBorderColor);
        Info.unnormalizedCoordinates = bUnnormalized ? VK_TRUE : VK_FALSE;
        
        check(vkCreateSampler(InDevice, &Info, nullptr, &Handle) == VK_SUCCESS, "FErolyssaSampler: failed to create!");
    }
    
    ~FErolyssaSampler()
    {
        if(Handle != VK_NULL_HANDLE) vkDestroySampler(Device, Handle, nullptr);
    }
    
    operator VkSampler() const { return Handle; }
    bool IsValid() const { return Handle != VK_NULL_HANDLE; }

private:
    const FErolyssaDevice& Device;
    VkSampler Handle = VK_NULL_HANDLE;
};
