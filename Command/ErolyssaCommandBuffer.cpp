// Root Engine / Erolyssa

#include "ErolyssaCommandBuffer.h"
#include "ErolyssaPhysicalDevice.h"
#include "ErolyssaBuffer.h"
#include <cstdio>
#include <cstdlib>

#include "Erolyssa.h"

FErolyssaCommandBuffer::FErolyssaCommandBuffer(const FErolyssaPhysicalDevice& InDevice)
    : LogicalDeviceRef(InDevice.GetLogicalHandle())
{
    // Шаг 1: Создаем Пул команд (Command Pool)
    VkCommandPoolCreateInfo PoolInfo{};
    PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // Пулы жестко привязываются к индексу семейства очередей
    PoolInfo.queueFamilyIndex = InDevice.GetComputeQueueFamilyIndex();
    // Флаг позволяет нам полностью перезаписывать буфер команд заново каждый кадр
    PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(LogicalDeviceRef, &PoolInfo, nullptr, &CommandPoolHandle) != VK_SUCCESS)
    {
        ErolyssaTerminate("FErolyssaCommandBuffer: Не удалось создать VkCommandPool!");
    }

    // Шаг 2: Выделяем Буфер команд (Command Buffer) из созданного пула
    VkCommandBufferAllocateInfo AllocInfo{};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.commandPool = CommandPoolHandle;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Первичный буфер (может отправляться в очередь)
    AllocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(LogicalDeviceRef, &AllocInfo, &CommandBufferHandle) != VK_SUCCESS)
    {
        ErolyssaTerminate("FErolyssaCommandBuffer: Не удалось выделить VkCommandBuffer!");
    }
}

FErolyssaCommandBuffer::~FErolyssaCommandBuffer()
{
    // При уничтожении пула все выделенные из него буферы команд освобождаются автоматически
    if (CommandPoolHandle != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(LogicalDeviceRef, CommandPoolHandle, nullptr);
    }
}

void FErolyssaCommandBuffer::Begin()
{
    VkCommandBufferBeginInfo BeginInfo{};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // Говорим драйверу, что мы запишем команды, выполним их и, возможно, сразу перезапишем
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(CommandBufferHandle, &BeginInfo) != VK_SUCCESS)
    {
        ErolyssaTerminate("FErolyssaCommandBuffer: Не удалось начать запись буфера команд!");
    }
}

void FErolyssaCommandBuffer::End()
{
    if (vkEndCommandBuffer(CommandBufferHandle) != VK_SUCCESS)
    {
        ErolyssaTerminate("FErolyssaCommandBuffer: Не удалось завершить запись буфера команд!");
    }
}

void FErolyssaCommandBuffer::CopyBufferCPU(const FErolyssaBuffer& SourceBuffer, const FErolyssaBuffer& DestinationBuffer)
{
    VkBufferCopy CopyRegion{};
    CopyRegion.srcOffset = 0;
    CopyRegion.dstOffset = 0;
    CopyRegion.size = SourceBuffer.GetSize();
    
    vkCmdCopyBuffer(CommandBufferHandle, SourceBuffer.GetHandle(), DestinationBuffer.GetHandle(), 1, &CopyRegion);
}

void FErolyssaCommandBuffer::SubmitAndWait(VkQueue QueueHandle)
{
    VkSubmitInfo SubmitInfo{};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBufferHandle;
    
    if (vkQueueSubmit(QueueHandle, 1, &SubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        ErolyssaTerminate("FErolyssaCommandBuffer: Ошибка при отправке (Submit) буфера команд в очередь!");
    }
    
    vkQueueWaitIdle(QueueHandle);
}
