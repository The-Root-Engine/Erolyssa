// Root Engine / Erolyssa

#pragma once

#define EROLYSSA_ENABLE_VALIDATION_LAYERS 1
#define EROLYSSA_VLA(Type, Size) static_cast<Type*>(_alloca(Size * sizeof(Type)))

inline uint32 ErolyssaFindMemoryType(const VkPhysicalDevice InPhysical, const uint32 InTypeFilter, const VkMemoryPropertyFlags InProperties)
{
    VkPhysicalDeviceMemoryProperties MemProps{};
    vkGetPhysicalDeviceMemoryProperties(InPhysical, &MemProps);
        
    for(uint32 i = 0; i < MemProps.memoryTypeCount; ++i)
        if((InTypeFilter & (1u << i)) && (MemProps.memoryTypes[i].propertyFlags & InProperties) == InProperties)
            return i;
        
    FErolyssaDebug::Terminate("FErolyssaBuffer: no suitable memory type found!");
    return 0xFFFFFFFF;
}
