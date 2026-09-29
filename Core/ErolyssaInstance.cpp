// Root Engine / Erolyssa

#include "ErolyssaInstance.h"

#include "Erolyssa.h"

FErolyssaInstance::FErolyssaInstance()
{
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
    if(!ValidationLayerCheck())
        ErolyssaTerminate("Validation Layer is not supported!");
#endif
    
    VkApplicationInfo AppInfo{};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = "UnRootedVoxels";
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "Erolyssa";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_3;
    
    VkInstanceCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    CreateInfo.pNext = nullptr;
    CreateInfo.pApplicationInfo = &AppInfo;
    
    auto Extensions = GetRequiredExtensions();
    CreateInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
    CreateInfo.ppEnabledExtensionNames = Extensions.data();
    
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
    VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo{};
    DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    DebugCreateInfo.pfnUserCallback = ValidationLayerDebugCallback;
    
    const std::vector<const char*> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    
    CreateInfo.enabledLayerCount = 1;
    CreateInfo.ppEnabledLayerNames = ValidationLayers.data();
    CreateInfo.pNext = &DebugCreateInfo;
#else
    CreateInfo.enabledLayerCount = 0;
#endif
    
    if(vkCreateInstance(&CreateInfo, nullptr, &Handle) != VK_SUCCESS)
        ErolyssaTerminate("Failed to create ErolyssaInstance!");
    
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
    ValidationLayerSetup();
#endif
}

FErolyssaInstance::~FErolyssaInstance() 
{
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
    if(ValidationLayerDebugMessenger != VK_NULL_HANDLE)
        if(const PFN_vkDestroyDebugUtilsMessengerEXT Func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(Handle, "vkDestroyDebugUtilsMessengerEXT")))
            Func(Handle, ValidationLayerDebugMessenger, nullptr);
#endif
    
    if(Handle != VK_NULL_HANDLE)
        vkDestroyInstance(Handle, nullptr);
}

bool FErolyssaInstance::ValidationLayerCheck() 
{
    uint32_t LayersCount;
    vkEnumerateInstanceLayerProperties(&LayersCount, nullptr);
    VkLayerProperties* Layers = EROLYSSA_VLA(VkLayerProperties, LayersCount);
    vkEnumerateInstanceLayerProperties(&LayersCount, Layers);
    
    bool bLayerFound = false;
    for(uint32_t i = 0; i < LayersCount; ++i)
    {
        if(strcmp("VK_LAYER_KHRONOS_validation", Layers[i].layerName) == 0) 
        {
            bLayerFound = true;
            break;
        }
    }
    if(!bLayerFound) return false;
    
    return true;
}

std::vector<const char*> FErolyssaInstance::GetRequiredExtensions() 
{
    std::vector<const char*> Extensions;
    
#if EROLYSSA_ENABLE_VALIDATION_LAYERS
    Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    
    return Extensions;
}

void FErolyssaInstance::ValidationLayerSetup() 
{
    VkDebugUtilsMessengerCreateInfoEXT CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    CreateInfo.pfnUserCallback = ValidationLayerDebugCallback;
    
    if(const PFN_vkCreateDebugUtilsMessengerEXT Func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(Handle, "vkCreateDebugUtilsMessengerEXT"))) 
        if (Func(Handle, &CreateInfo, nullptr, &ValidationLayerDebugMessenger) != VK_SUCCESS) 
            ErolyssaTerminate("FErolyssaInstance: Не удалось настроить отладочный мессенджер!");
}

VKAPI_ATTR VkBool32 VKAPI_CALL FErolyssaInstance::ValidationLayerDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData) 
{
    if(MessageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) ErolyssaLogValidation(CallbackData->pMessage);
    return VK_FALSE;
}
