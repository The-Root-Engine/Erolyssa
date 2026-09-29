#include "ErolyssaDescriptorPool.h"

#include "ErolyssaPhysicalDevice.h"

FErolyssaDescriptorPool::~FErolyssaDescriptorPool()
{
    if(Handle != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(Device.GetLogicalHandle(), Handle, nullptr);
}
