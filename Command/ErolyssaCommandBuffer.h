// Root Engine / Erolyssa

#pragma once

#include <vulkan/vulkan.h>

class FErolyssaPhysicalDevice;
class FErolyssaBuffer;

class FErolyssaCommandBuffer
{
public:
    FErolyssaCommandBuffer(const FErolyssaPhysicalDevice& InDevice);
    ~FErolyssaCommandBuffer();

    // Запрет копирования
    FErolyssaCommandBuffer(const FErolyssaCommandBuffer&) = delete;
    FErolyssaCommandBuffer& operator=(const FErolyssaCommandBuffer&) = delete;
    
    void Begin();
    void End();
    void CopyBufferCPU(const FErolyssaBuffer& SourceBuffer, const FErolyssaBuffer& DestinationBuffer);
    
    void SubmitAndWait(VkQueue QueueHandle);
    
    VkCommandBuffer& GetCommandBufferHandle() { return CommandBufferHandle; }

private:
    VkDevice LogicalDeviceRef = VK_NULL_HANDLE;
    VkCommandPool CommandPoolHandle = VK_NULL_HANDLE;
    VkCommandBuffer CommandBufferHandle = VK_NULL_HANDLE;
};
