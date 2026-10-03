// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../ErolyssaAliases.h"
#include "../Erolyssa.h"

enum class EErolyssaVersion
{
    Vulkan_1_0, Vulkan_1_1, Vulkan_1_2, Vulkan_1_3, Vulkan_1_4
};

class FErolyssaInstance 
{
    
public:
    explicit FErolyssaInstance(const EErolyssaVersion InVersion, const TArray<const char*>& InRequiredExtensions, const TArray<const char*>& InRequiredLayers)
    {
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
        uint32 LayersCount;
        vkEnumerateInstanceLayerProperties(&LayersCount, nullptr);
        VkLayerProperties* LayerProperties = EROLYSSA_VLA(VkLayerProperties, LayersCount);
        vkEnumerateInstanceLayerProperties(&LayersCount, LayerProperties);
        
        bool bLayerFound = false;
        for(uint32 i = 0; i < LayersCount; ++i)
            if(strcmp("VK_LAYER_KHRONOS_validation", LayerProperties[i].layerName) == 0) 
            {
                bLayerFound = true;
                break;
            }
        
        check(bLayerFound, "Validation Layer is not supported!");
#endif
        
        VkApplicationInfo AppInfo{};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pApplicationName = "Erolyssa";
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = "Root Engine";
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.apiVersion = ToVulkan(InVersion);
        
        TArray<const char*> Extensions;
        {
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
            Extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
            
            for(const char* Extension : InRequiredExtensions)
                Extensions.Add(Extension);
        }
        
        TArray<const char*> Layers;
        {
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
            Layers.Add("VK_LAYER_KHRONOS_validation");
#endif
            
            for(const char* L : InRequiredLayers)
                Layers.Add(L);
        }
        
        VkInstanceCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        CreateInfo.pNext = nullptr;
        CreateInfo.pApplicationInfo = &AppInfo;
        CreateInfo.enabledExtensionCount = static_cast<uint32>(Extensions.Num());
        CreateInfo.ppEnabledExtensionNames = Extensions.Data();
        CreateInfo.enabledLayerCount       = static_cast<uint32>(Layers.Num());
        CreateInfo.ppEnabledLayerNames     = Layers.Data();
        
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo{};
        DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugCreateInfo.pfnUserCallback = ValidationLayerDebugCallback;
        
        CreateInfo.pNext = &DebugCreateInfo;
#endif
        
        check(vkCreateInstance(&CreateInfo, nullptr, &Handle) == VK_SUCCESS, "Failed to create ErolyssaInstance!");
        
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerCreateInfoEXT ValidationCreateInfo{};
        ValidationCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        ValidationCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        ValidationCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        ValidationCreateInfo.pfnUserCallback = ValidationLayerDebugCallback;
        
        if(const PFN_vkCreateDebugUtilsMessengerEXT Func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(Handle, "vkCreateDebugUtilsMessengerEXT"))) 
            check(Func(Handle, &ValidationCreateInfo, nullptr, &ValidationLayerDebugMessenger) == VK_SUCCESS, "Failed to setup Validation Messanger!");
#endif
    }
    
    ~FErolyssaInstance()
    {
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
        if(ValidationLayerDebugMessenger != VK_NULL_HANDLE)
            if(const PFN_vkDestroyDebugUtilsMessengerEXT Func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(Handle, "vkDestroyDebugUtilsMessengerEXT")))
                Func(Handle, ValidationLayerDebugMessenger, nullptr);
#endif
        
        if(Handle != VK_NULL_HANDLE) vkDestroyInstance(Handle, nullptr);
    }
    
    FErolyssaInstance(const FErolyssaInstance&) = delete;
    FErolyssaInstance& operator=(const FErolyssaInstance&) = delete;
    
    operator VkInstance() const { return Handle; }

private:
    VkInstance Handle = VK_NULL_HANDLE;
    
    static uint32 ToVulkan(const EErolyssaVersion InApiVersion)
    {
        switch (InApiVersion)
        {
        case EErolyssaVersion::Vulkan_1_0: return VK_API_VERSION_1_0;
        case EErolyssaVersion::Vulkan_1_1: return VK_API_VERSION_1_1;
        case EErolyssaVersion::Vulkan_1_2: return VK_API_VERSION_1_2;
        case EErolyssaVersion::Vulkan_1_3: return VK_API_VERSION_1_3;
        case EErolyssaVersion::Vulkan_1_4: return VK_API_VERSION_1_4;
        }
        return VK_API_VERSION_1_0;
    }
    
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
    VkDebugUtilsMessengerEXT ValidationLayerDebugMessenger = VK_NULL_HANDLE;
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayerDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData)
    {
        if(MessageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) FErolyssaDebug::Validation(CallbackData->pMessage);
        return VK_FALSE;
    }
#endif
};
