// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../ErolyssaAliases.h"

enum class EErolyssaCullMode : uint8
{
    None  = 0,
    Front = 1 << 0,
    Back  = 1 << 1,
};
ENUM_CLASS_FLAGS(EErolyssaCullMode)

enum class EErolyssaPolygonMode : uint8
{
    Fill  = 0,
    Line  = 1,
    Point = 2,
};

enum class EErolyssaFrontFace : uint8
{
    Clockwise        = 0,
    CounterClockwise = 1,
};

enum class EErolyssaCompareOp : uint8
{
    Never          = 0,
    Less           = 1,
    Equal          = 2,
    LessOrEqual    = 3,
    Greater        = 4,
    NotEqual       = 5,
    GreaterOrEqual = 6,
    Always         = 7,
};

inline VkCullModeFlags ToVulkan(EErolyssaCullMode InMode)
{
    VkCullModeFlags Result = 0;
    if(EnumHasAnyFlags(InMode, EErolyssaCullMode::Front)) Result |= VK_CULL_MODE_FRONT_BIT;
    if(EnumHasAnyFlags(InMode, EErolyssaCullMode::Back))  Result |= VK_CULL_MODE_BACK_BIT;
    return Result;
}

inline VkPolygonMode ToVulkan(EErolyssaPolygonMode InMode)
{
    switch(InMode)
    {
    case EErolyssaPolygonMode::Fill:  return VK_POLYGON_MODE_FILL;
    case EErolyssaPolygonMode::Line:  return VK_POLYGON_MODE_LINE;
    case EErolyssaPolygonMode::Point: return VK_POLYGON_MODE_POINT;
    }
    return VK_POLYGON_MODE_FILL;
}

inline VkFrontFace ToVulkan(EErolyssaFrontFace InFace)
{
    switch(InFace)
    {
    case EErolyssaFrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
    case EErolyssaFrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
    return VK_FRONT_FACE_CLOCKWISE;
}

inline VkCompareOp ToVulkan(EErolyssaCompareOp InOp)
{
    switch(InOp)
    {
    case EErolyssaCompareOp::Never:          return VK_COMPARE_OP_NEVER;
    case EErolyssaCompareOp::Less:           return VK_COMPARE_OP_LESS;
    case EErolyssaCompareOp::Equal:          return VK_COMPARE_OP_EQUAL;
    case EErolyssaCompareOp::LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
    case EErolyssaCompareOp::Greater:        return VK_COMPARE_OP_GREATER;
    case EErolyssaCompareOp::NotEqual:       return VK_COMPARE_OP_NOT_EQUAL;
    case EErolyssaCompareOp::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case EErolyssaCompareOp::Always:         return VK_COMPARE_OP_ALWAYS;
    }
    return VK_COMPARE_OP_ALWAYS;
}

template<
    EErolyssaPolygonMode PolygonMode,
    EErolyssaCullMode CullMode,
    EErolyssaFrontFace FrontFace = EErolyssaFrontFace::CounterClockwise
>
struct TErolyssaPipelineStateRasterization
{
    
public:
    static void Build(VkPipelineRasterizationStateCreateInfo& OutInfo)
    {
        OutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        OutInfo.depthClampEnable        = VK_FALSE;
        OutInfo.rasterizerDiscardEnable = VK_FALSE;
        OutInfo.polygonMode             = ToVulkan(PolygonMode);
        OutInfo.cullMode                = ToVulkan(CullMode);
        OutInfo.frontFace               = ToVulkan(FrontFace);
        OutInfo.depthBiasEnable         = VK_FALSE;
        OutInfo.depthBiasConstantFactor = 0.0f;
        OutInfo.depthBiasClamp          = 0.0f;
        OutInfo.depthBiasSlopeFactor    = 0.0f;
        OutInfo.lineWidth               = 1.0f;
    }
};

template<
    EErolyssaCompareOp CompareOp,
    bool bDepthTest  = true,
    bool bDepthWrite = true
>
struct TErolyssaPipelineStateDepthStencil
{
    static void Build(VkPipelineDepthStencilStateCreateInfo& OutInfo)
    {
        OutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        OutInfo.depthTestEnable       = bDepthTest  ? VK_TRUE : VK_FALSE;
        OutInfo.depthWriteEnable      = bDepthWrite ? VK_TRUE : VK_FALSE;
        OutInfo.depthCompareOp        = ToVulkan(CompareOp);
        OutInfo.depthBoundsTestEnable = VK_FALSE;
        OutInfo.stencilTestEnable     = VK_FALSE;
        OutInfo.minDepthBounds        = 0.0f;
        OutInfo.maxDepthBounds        = 1.0f;
    }
};

enum class EErolyssaSampleCount : uint8
{
    Count1  = 1,
    Count2  = 2,
    Count4  = 4,
    Count8  = 8,
    Count16 = 16,
    Count32 = 32,
    Count64 = 64,
};

inline VkSampleCountFlagBits ToVulkan(const EErolyssaSampleCount InSampleCount)
{
    switch(InSampleCount)
    {
    case EErolyssaSampleCount::Count1:  return VK_SAMPLE_COUNT_1_BIT;
    case EErolyssaSampleCount::Count2:  return VK_SAMPLE_COUNT_2_BIT;
    case EErolyssaSampleCount::Count4:  return VK_SAMPLE_COUNT_4_BIT;
    case EErolyssaSampleCount::Count8:  return VK_SAMPLE_COUNT_8_BIT;
    case EErolyssaSampleCount::Count16: return VK_SAMPLE_COUNT_16_BIT;
    case EErolyssaSampleCount::Count32: return VK_SAMPLE_COUNT_32_BIT;
    case EErolyssaSampleCount::Count64: return VK_SAMPLE_COUNT_64_BIT;
    }
    return VK_SAMPLE_COUNT_1_BIT;
}

enum class EErolyssaPrimitiveTopology : uint8
{
    PointList     = 0,
    LineList      = 1,
    LineStrip     = 2,
    TriangleList  = 3,
    TriangleStrip = 4,
    TriangleFan   = 5,
    LineListWithAdjacency     = 6,
    LineStripWithAdjacency    = 7,
    TriangleListWithAdjacency = 8,
    TriangleStripWithAdjacency = 9,
    PatchList = 10,
};

inline VkPrimitiveTopology ToVulkan(const EErolyssaPrimitiveTopology InPrimitiveTopology)
{
    switch(InPrimitiveTopology)
    {
    case EErolyssaPrimitiveTopology::PointList:                  return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case EErolyssaPrimitiveTopology::LineList:                   return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case EErolyssaPrimitiveTopology::LineStrip:                  return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case EErolyssaPrimitiveTopology::TriangleList:               return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case EErolyssaPrimitiveTopology::TriangleStrip:              return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case EErolyssaPrimitiveTopology::TriangleFan:                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    case EErolyssaPrimitiveTopology::LineListWithAdjacency:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
    case EErolyssaPrimitiveTopology::LineStripWithAdjacency:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
    case EErolyssaPrimitiveTopology::TriangleListWithAdjacency:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
    case EErolyssaPrimitiveTopology::TriangleStripWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
    case EErolyssaPrimitiveTopology::PatchList:                  return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
    }
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

template<
    EErolyssaSampleCount Samples = EErolyssaSampleCount::Count1,
    bool bSampleShading = false
>
struct TErolyssaPipelineStateMultisample
{
    static void Build(VkPipelineMultisampleStateCreateInfo& OutInfo)
    {
        OutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        OutInfo.sampleShadingEnable   = bSampleShading ? VK_TRUE : VK_FALSE;
        OutInfo.rasterizationSamples  = ToVulkan(Samples);
        OutInfo.minSampleShading      = 1.0f;
        OutInfo.pSampleMask           = nullptr;
        OutInfo.alphaToCoverageEnable = VK_FALSE;
        OutInfo.alphaToOneEnable      = VK_FALSE;
    }
};

template<
    EErolyssaPrimitiveTopology Topology = EErolyssaPrimitiveTopology::TriangleList,
    bool bPrimitiveRestart = false
>
struct TErolyssaPipelineStateInputAssembly
{
    static void Build(VkPipelineInputAssemblyStateCreateInfo& OutInfo)
    {
        OutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        OutInfo.topology               = ToVulkan(Topology);
        OutInfo.primitiveRestartEnable = bPrimitiveRestart ? VK_TRUE : VK_FALSE;
    }
};

enum class EErolyssaColorWriteMask : uint8
{
    None = 0,
    R    = 1 << 0,
    G    = 1 << 1,
    B    = 1 << 2,
    A    = 1 << 3,
    RGBA = R | G | B | A,
    RGB  = R | G | B,
};
ENUM_CLASS_FLAGS(EErolyssaColorWriteMask)

inline VkColorComponentFlags ToVulkan(const EErolyssaColorWriteMask InMask)
{
    VkColorComponentFlags Result = 0;
    if(EnumHasAnyFlags(InMask, EErolyssaColorWriteMask::R)) Result |= VK_COLOR_COMPONENT_R_BIT;
    if(EnumHasAnyFlags(InMask, EErolyssaColorWriteMask::G)) Result |= VK_COLOR_COMPONENT_G_BIT;
    if(EnumHasAnyFlags(InMask, EErolyssaColorWriteMask::B)) Result |= VK_COLOR_COMPONENT_B_BIT;
    if(EnumHasAnyFlags(InMask, EErolyssaColorWriteMask::A)) Result |= VK_COLOR_COMPONENT_A_BIT;
    return Result;
}

enum class EErolyssaBlendMode : uint8
{
    Opaque     = 0,
    AlphaBlend = 1,
    Additive   = 2,
    Multiply   = 3,
};

template<
    EErolyssaBlendMode BlendMode = EErolyssaBlendMode::Opaque,
    EErolyssaColorWriteMask WriteMask = EErolyssaColorWriteMask::RGBA
>
struct TErolyssaPipelineStateAttachment
{
    static void Build(VkPipelineColorBlendAttachmentState& OutAttachment)
    {
        OutAttachment = {};
        OutAttachment.colorWriteMask = ToVulkan(WriteMask);
        OutAttachment.blendEnable = VK_FALSE;
        
        switch(BlendMode)
        {
            case EErolyssaBlendMode::Opaque:
                OutAttachment.blendEnable = VK_FALSE;
                break;
                
            case EErolyssaBlendMode::AlphaBlend:
                OutAttachment.blendEnable         = VK_TRUE;
                OutAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                OutAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                OutAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
                OutAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                OutAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                OutAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
                break;
                
            case EErolyssaBlendMode::Additive:
                OutAttachment.blendEnable         = VK_TRUE;
                OutAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                OutAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
                OutAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
                OutAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                OutAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                OutAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
                break;
                
            case EErolyssaBlendMode::Multiply:
                OutAttachment.blendEnable         = VK_TRUE;
                OutAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
                OutAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
                OutAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
                OutAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
                OutAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                OutAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
                break;
        }
    }
};

template<typename... ColorBlendAttachments>
struct TErolyssaPipelineStateColorBlend
{
    
public:
    static constexpr size_t Count = sizeof...(ColorBlendAttachments);
    
    static void Build(
        VkPipelineColorBlendStateCreateInfo& OutState,
        TStaticArray<VkPipelineColorBlendAttachmentState, Count>& OutAttachments)
    {
        BuildAll(OutAttachments, Meta::MakeIndexSequence<sizeof...(ColorBlendAttachments)>{});
        
        OutState = {};
        OutState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        OutState.logicOpEnable = VK_FALSE;
        OutState.attachmentCount = static_cast<uint32>(Count);
        OutState.pAttachments = OutAttachments.Data();
        OutState.blendConstants[0] = 0.0f;
        OutState.blendConstants[1] = 0.0f;
        OutState.blendConstants[2] = 0.0f;
        OutState.blendConstants[3] = 0.0f;
    }

private:
    template<size_t... Is>
    static void BuildAll(
        TStaticArray<VkPipelineColorBlendAttachmentState, Count>& OutAttachments,
        Meta::IndexSequence<Is...>)
    {
        (ColorBlendAttachments::Build(OutAttachments[Is]), ...);
    }
};

template<
    typename StateRasterization,
    typename StateDepthStencil,
    typename StateMultisample,
    typename StateInputAssembly,
    typename StateColorBlend
>
class FErolyssaPipelineBuilder
{
    
public:
    void Build(VkGraphicsPipelineCreateInfo& OutPipelineInfo)
    {
        StateRasterization::Build(RasterizationInfo);
        StateDepthStencil::Build(DepthStencilInfo);
        StateMultisample::Build(MultisampleInfo);
        StateInputAssembly::Build(InputAssemblyInfo);
        StateColorBlend::Build(ColorBlendInfo, ColorBlendAttachments);
        
        OutPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        
        OutPipelineInfo.pRasterizationState = &RasterizationInfo;
        OutPipelineInfo.pDepthStencilState  = &DepthStencilInfo;
        OutPipelineInfo.pMultisampleState   = &MultisampleInfo;
        OutPipelineInfo.pInputAssemblyState = &InputAssemblyInfo;
        OutPipelineInfo.pColorBlendState    = &ColorBlendInfo;
    }

private:
    VkPipelineRasterizationStateCreateInfo RasterizationInfo{};
    VkPipelineDepthStencilStateCreateInfo  DepthStencilInfo{};
    VkPipelineMultisampleStateCreateInfo   MultisampleInfo{};
    VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo{};
    VkPipelineColorBlendStateCreateInfo    ColorBlendInfo{};
    TStaticArray<VkPipelineColorBlendAttachmentState, StateColorBlend::Count> ColorBlendAttachments{};
};
