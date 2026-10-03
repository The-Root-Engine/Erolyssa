// Root Engine / Erolyssa

#pragma once

enum class EErolyssaShaderType : uint8
{
    Vertex      = 0,
    Fragment    = 1,
    Compute     = 2,
    Geometry    = 3,
};

inline VkShaderStageFlagBits ToVulkan(const EErolyssaShaderType InType)
{
    switch(InType)
    {
    case EErolyssaShaderType::Vertex:      return VK_SHADER_STAGE_VERTEX_BIT;
    case EErolyssaShaderType::Fragment:    return VK_SHADER_STAGE_FRAGMENT_BIT;
    case EErolyssaShaderType::Compute:     return VK_SHADER_STAGE_COMPUTE_BIT;
    case EErolyssaShaderType::Geometry:    return VK_SHADER_STAGE_GEOMETRY_BIT;
    }
    return VK_SHADER_STAGE_VERTEX_BIT;
}

struct FErolyssaShaderBinding
{

public:
    uint32 Binding;
};

struct FErolyssaShader
{
    
public:
    static constexpr const char* GetSourcePath() { return ""; }
    static constexpr const char* GetFuncName() { return "main"; }
    static constexpr EErolyssaShaderType GetType() { return EErolyssaShaderType::Compute; }

private:
};
