// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
// #include "../ErolyssaAliases.h"
// #include "../Resources/ErolyssaBuffer.h"
#include "FErolyssaCommandPool.h"

class FErolyssaCommandBuffer
{
public:
    FErolyssaCommandBuffer(
        const FErolyssaCommandPool& InPool,
        const EErolyssaCommandBufferLevel InLevel = EErolyssaCommandBufferLevel::Primary
    ) : Pool(InPool)
    {
        Handle = Pool.Allocate(InLevel);
    }
    
    ~FErolyssaCommandBuffer()
    {
        if(Handle != VK_NULL_HANDLE) Pool.Free(Handle);
    }
    
    FErolyssaCommandBuffer(const FErolyssaCommandBuffer&) = delete;
    FErolyssaCommandBuffer& operator=(const FErolyssaCommandBuffer&) = delete;
    
    operator VkCommandBuffer() const { return Handle; }
    operator const VkCommandBuffer*() const { return &Handle; }
    
    const FErolyssaCommandPool& GetPool() const { return Pool; }
    
    void Reset(const VkCommandBufferResetFlags InFlags = 0) const
    {
        vkResetCommandBuffer(Handle, InFlags);
    }
    
    void Begin(const VkCommandBufferUsageFlags InFlags = 0) const
    {
        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = InFlags;
        
        check(vkBeginCommandBuffer(Handle, &BeginInfo) == VK_SUCCESS, "FErolyssaCommandBuffer: failed to begin!");
    }
    
    void End() const
    {
        check(vkEndCommandBuffer(Handle) == VK_SUCCESS, "FErolyssaCommandBuffer: failed to end!");
    }
    
    /*
    void CopyBuffer(
        const FErolyssaBuffer& InSrc, const FErolyssaBuffer& InDst,
        const FErolyssaSize InSize, const FErolyssaSize InSrcOffset = 0, const FErolyssaSize InDstOffset = 0
    )
    {
        VkBufferCopy CopyRegion{};
        CopyRegion.srcOffset = InSrcOffset;
        CopyRegion.dstOffset = InDstOffset;
        CopyRegion.size = InSize;
        
        vkCmdCopyBuffer(Handle, InSrc, InDst, 1, &CopyRegion);
    }
    */

private:
    const FErolyssaCommandPool& Pool;
    VkCommandBuffer Handle = VK_NULL_HANDLE;
};
