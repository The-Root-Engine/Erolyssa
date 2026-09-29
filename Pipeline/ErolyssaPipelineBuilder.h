// Root Engine / Erolyssa

#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

class FErolyssaPipeline;
class FErolyssaPhysicalDevice;

class FErolyssaPipelineBuilder 
{
    
public:
    FErolyssaPipelineBuilder(const FErolyssaPhysicalDevice& InDevice, FErolyssaPipeline& OutPipeline) : Device(InDevice), Pipeline(OutPipeline) {}
    ~FErolyssaPipelineBuilder() = default;
    
    FErolyssaPipelineBuilder& SetShader(const char* Path) {
        ShaderPath = Path;
        return *this;
    }
    
    FErolyssaPipelineBuilder& AddBinding(VkDescriptorType Type) {
        BindingTypes.push_back(Type);
        return *this;
    }
    
    FErolyssaPipelineBuilder& SetPushConstantSize(uint32_t Size) {
        PushConstantSize = Size;
        return *this;
    }
    
    void Build();

private:
    const FErolyssaPhysicalDevice& Device;
    FErolyssaPipeline& Pipeline;
    const char* ShaderPath = nullptr;
    std::vector<VkDescriptorType> BindingTypes;
    uint32_t PushConstantSize = 0;
    
    static std::vector<char> ReadShaderFile(const char* FilePath);
};
