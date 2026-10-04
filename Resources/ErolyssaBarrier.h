// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../Erolyssa.h"
#include "../Commands/ErolyssaCommandBuffer.h"
#include "../Core/ErolyssaDevice.h"

enum class FErolyssaBarrierResourceState : uint16
{
    None                   = 0,
    VertexRead             = 1 << 0,
    IndexRead              = 1 << 1,
    UniformRead            = 1 << 2,
    ShaderRead             = 1 << 3,  // SRV / Sampled / Storage Read
    ShaderWrite            = 1 << 4,  // UAV / Storage Write
    IndirectBuffer         = 1 << 5,  // Для Indirect аргументов
    ColorAttachment        = 1 << 6,  // Render Target
    DepthStencilAttachment = 1 << 7,  // Z-Buffer
    TransferSrc            = 1 << 8,
    TransferDst            = 1 << 9,
    Present                = 1 << 10  // Swapchain вывод
};
ENUM_CLASS_FLAGS(FErolyssaBarrierResourceState)

inline void ToVulkan(const FErolyssaBarrierResourceState State, VkPipelineStageFlags2& OutStage, VkAccessFlags2& OutAccess, VkImageLayout* OutLayout = nullptr)
{
    OutStage = 0;
    OutAccess = 0;
    if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_GENERAL;
    
    if(State == FErolyssaBarrierResourceState::None)
    {
        OutStage = VK_PIPELINE_STAGE_2_NONE;
        if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        return;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::VertexRead))
    {
        OutStage |= VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;
        OutAccess |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::IndexRead))
    {
        OutStage |= VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;
        OutAccess |= VK_ACCESS_2_INDEX_READ_BIT;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::UniformRead))
    {
        OutStage |= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        OutAccess |= VK_ACCESS_2_UNIFORM_READ_BIT;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::ShaderRead))
    {
        OutStage |= VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        OutAccess |= VK_ACCESS_2_SHADER_READ_BIT;
        if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::ShaderWrite))
    {
        OutStage |= VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        OutAccess |= VK_ACCESS_2_SHADER_WRITE_BIT;
        if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_GENERAL;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::IndirectBuffer))
    {
        OutStage |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
        OutAccess |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::ColorAttachment))
    {
        OutStage |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        OutAccess |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::DepthStencilAttachment))
    {
        OutStage |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        OutAccess |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::TransferSrc))
    {
        OutStage |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        OutAccess |= VK_ACCESS_2_TRANSFER_READ_BIT;
        if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::TransferDst))
    {
        OutStage |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        OutAccess |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
        if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }
    
    if(EnumHasAnyFlags(State, FErolyssaBarrierResourceState::Present))
    {
        OutStage |= VK_PIPELINE_STAGE_2_NONE;
        if(OutLayout) *OutLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
}
struct FErolyssaBarrierBufferInfo
{
    VkBuffer Buffer;
    usize Offset;
    usize Size;
    FErolyssaBarrierResourceState OldState;
    FErolyssaBarrierResourceState NewState;
};

struct FErolyssaBarrierTextureInfo
{
    VkImage Image;
    VkImageAspectFlags AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    FErolyssaBarrierResourceState OldState;
    FErolyssaBarrierResourceState NewState;
};

class FErolyssaBarrier
{
    
public:
    static void Insert(
        const FErolyssaCommandBuffer& InCommandBuffer,
        const TArray<FErolyssaBarrierBufferInfo>& InBufferBarriers,
        const TArray<FErolyssaBarrierTextureInfo>& InTextureBarriers
    )
    {
        VkBufferMemoryBarrier2* VkBufferBarriers = EROLYSSA_VLA(VkBufferMemoryBarrier2, InBufferBarriers.Num());
        
        for(uint32 i = 0; i < InBufferBarriers.Num(); ++i)
        {
            VkBufferBarriers[i] = {};
            VkBufferBarriers[i].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
            VkBufferBarriers[i].buffer = InBufferBarriers[i].Buffer;
            VkBufferBarriers[i].offset = InBufferBarriers[i].Offset;
            VkBufferBarriers[i].size = InBufferBarriers[i].Size;
            VkBufferBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            VkBufferBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            
            ToVulkan(InBufferBarriers[i].OldState, VkBufferBarriers[i].srcStageMask, VkBufferBarriers[i].srcAccessMask);
            ToVulkan(InBufferBarriers[i].NewState, VkBufferBarriers[i].dstStageMask, VkBufferBarriers[i].dstAccessMask);
        }
        
        VkImageMemoryBarrier2* VkImageBarriers = EROLYSSA_VLA(VkImageMemoryBarrier2, InTextureBarriers.Num());
        for(uint32 i = 0; i < InTextureBarriers.Num(); ++i)
        {
            VkImageBarriers[i] = {};
            VkImageBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            VkImageBarriers[i].image = InTextureBarriers[i].Image;
            VkImageBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            VkImageBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            VkImageBarriers[i].subresourceRange.aspectMask = InTextureBarriers[i].AspectMask;
            VkImageBarriers[i].subresourceRange.baseMipLevel = 0;
            VkImageBarriers[i].subresourceRange.levelCount = 1;
            VkImageBarriers[i].subresourceRange.baseArrayLayer = 0;
            VkImageBarriers[i].subresourceRange.layerCount = 1;
            
            ToVulkan(InTextureBarriers[i].OldState, VkImageBarriers[i].srcStageMask, VkImageBarriers[i].srcAccessMask, &VkImageBarriers[i].oldLayout);
            ToVulkan(InTextureBarriers[i].NewState, VkImageBarriers[i].dstStageMask, VkImageBarriers[i].dstAccessMask, &VkImageBarriers[i].newLayout);
        }
        
        VkDependencyInfo DependencyInfo{};
        DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        DependencyInfo.bufferMemoryBarrierCount = static_cast<uint32>(InBufferBarriers.Num());
        DependencyInfo.pBufferMemoryBarriers = VkBufferBarriers;
        DependencyInfo.imageMemoryBarrierCount = static_cast<uint32>(InTextureBarriers.Num());
        DependencyInfo.pImageMemoryBarriers = VkImageBarriers;
        
        vkCmdPipelineBarrier2(InCommandBuffer, &DependencyInfo);
    }
};
