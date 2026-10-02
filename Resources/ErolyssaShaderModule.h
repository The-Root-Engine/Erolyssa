// Root Engine / Erolyssa

#pragma once

#include <fstream>

#include "../ErolyssaIncludeVulkan.h"

#include <vector>

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
        const std::vector<char> Code = ReadFile(InPath);
        VkShaderModuleCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        CreateInfo.codeSize = Code.size();
        CreateInfo.pCode = reinterpret_cast<const uint32_t*>(Code.data());
        
        check(vkCreateShaderModule(Device, &CreateInfo, nullptr, &Handle) == VK_SUCCESS, "FErolyssaShaderModule: failed to create shader module!");
    }
    
    operator VkShaderModule() const { return Handle; }

private:
    const FErolyssaDevice& Device;
    VkShaderModule Handle = VK_NULL_HANDLE;
    
    static std::vector<char> ReadFile(const char* InPath)
    {
        std::ifstream file(InPath, std::ios::ate | std::ios::binary);
        check(file.is_open(), "FErolyssaShaderModule: failed to open shader file: %s", InPath);

        const size_t FileSize = file.tellg();
        std::vector<char> Buffer(FileSize);
        
        file.seekg(0);
        file.read(Buffer.data(), FileSize);
        file.close();
        
        return Buffer;
    }
};
