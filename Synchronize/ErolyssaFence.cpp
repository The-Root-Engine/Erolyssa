#include "ErolyssaFence.h"

#include "Erolyssa.h"
#include "ErolyssaPhysicalDevice.h"

FErolyssaFence::FErolyssaFence(FErolyssaPhysicalDevice& InDevice) : Device(InDevice)
{
    VkFenceCreateInfo FenceInfo{};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if(vkCreateFence(Device.GetLogicalHandle(), &FenceInfo, nullptr, &Handle) != VK_SUCCESS)
        ErolyssaTerminate("Failed to create frame fence!");
}

FErolyssaFence::~FErolyssaFence()
{
    if(Handle != VK_NULL_HANDLE) vkDestroyFence(Device.GetLogicalHandle(), Handle, nullptr);
}

void FErolyssaFence::WaitFor()
{
    vkWaitForFences(Device.GetLogicalHandle(), 1, &Handle, VK_TRUE, UINT64_MAX);
}

void FErolyssaFence::Reset()
{
    vkResetFences(Device.GetLogicalHandle(), 1, &Handle);
}
