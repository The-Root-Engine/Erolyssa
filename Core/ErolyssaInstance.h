// Root Engine / Erolyssa

#pragma once

#include "Erolyssa.h"
#include <vulkan/vulkan.h>
#include <vector>

class FErolyssaInstance 
{
public:
    FErolyssaInstance();
    ~FErolyssaInstance();
    
    FErolyssaInstance(const FErolyssaInstance&) = delete;
    FErolyssaInstance& operator=(const FErolyssaInstance&) = delete;
    
    VkInstance GetHandle() const { return Handle; }

private:
    VkInstance Handle = VK_NULL_HANDLE;
    static std::vector<const char*> GetRequiredExtensions();
    
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
    VkDebugUtilsMessengerEXT ValidationLayerDebugMessenger = VK_NULL_HANDLE;
    
    static bool ValidationLayerCheck();
    void ValidationLayerSetup();
    static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayerDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData);
#endif
};
