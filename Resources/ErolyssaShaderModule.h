// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../ErolyssaAliases.h"

#include <fstream>

class FErolyssaDevice;

class FErolyssaShaderModule
{
    
public:
    FErolyssaShaderModule(const FErolyssaDevice& InDevice) : Device(InDevice) {}
    
    FErolyssaShaderModule(const FErolyssaDevice& InDevice, const char* InPath)
        : Device(InDevice) { Initialize(InPath); }
    
    ~FErolyssaShaderModule()
    {
        if(Handle != VK_NULL_HANDLE) vkDestroyShaderModule(Device, Handle, nullptr);
    }
    
    void Initialize(const char* InPath)
    {
        const TArray<char> Code = ReadFile(InPath);
        VkShaderModuleCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        CreateInfo.codeSize = Code.Num();
        CreateInfo.pCode = reinterpret_cast<const uint32*>(Code.Data());
        
        check(vkCreateShaderModule(Device, &CreateInfo, nullptr, &Handle) == VK_SUCCESS, "FErolyssaShaderModule: failed to create shader module!");
    }
    
    operator VkShaderModule() const { return Handle; }

private:
    const FErolyssaDevice& Device;
    VkShaderModule Handle = VK_NULL_HANDLE;
    
    static TArray<char> ReadFile(const char* InPath)
    {
        std::ifstream file(InPath, std::ios::ate | std::ios::binary);
        check(file.is_open(), "FErolyssaShaderModule: failed to open shader file: %s", InPath);
        
        const usize FileSize = file.tellg();
        TArray<char> Buffer(FileSize);
        
        file.seekg(0);
        file.read(Buffer.Data(), FileSize);
        file.close();
        
        return Buffer;
    }
};
