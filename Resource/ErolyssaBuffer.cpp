// Root Engine / Erolyssa

#include "ErolyssaBuffer.h"
#include "ErolyssaPhysicalDevice.h"
#include <cstdio>
#include <cstdlib>

#include "Erolyssa.h"

FErolyssaBuffer::FErolyssaBuffer(const FErolyssaPhysicalDevice& InDevice, VkDeviceSize InSize, VkBufferUsageFlags InUsage, VkMemoryPropertyFlags InProperties)
    : LogicalDeviceRef(InDevice.GetLogicalHandle()), Size(InSize)
{
    // Шаг 1: Создаем виртуальный объект буфера
    VkBufferCreateInfo BufferInfo{};
    BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferInfo.size = Size;
    BufferInfo.usage = InUsage;
    BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Используется только в нашей compute-очереди

    if (vkCreateBuffer(LogicalDeviceRef, &BufferInfo, nullptr, &BufferHandle) != VK_SUCCESS)
    {
        ErolyssaTerminate("FErolyssaBuffer: Не удалось создать VkBuffer!");
    }

    // Шаг 2: Запрашиваем у драйвера требования к памяти для этого конкретного буфера
    // GPU накладывает свои ограничения на выравнивание адресов (alignment)
    VkMemoryRequirements MemRequirements;
    vkGetBufferMemoryRequirements(LogicalDeviceRef, BufferHandle, &MemRequirements);

    // Шаг 3: Выделяем физическую память на устройстве
    VkMemoryAllocateInfo AllocInfo{};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemRequirements.size; // Драйвер может округлить наш Size в большую сторону из-за выравнивания
    AllocInfo.memoryTypeIndex = FindMemoryType(InDevice.GetPhysicalHandle(), MemRequirements.memoryTypeBits, InProperties);

    if (vkAllocateMemory(LogicalDeviceRef, &AllocInfo, nullptr, &MemoryHandle) != VK_SUCCESS)
    {
        ErolyssaTerminate("FErolyssaBuffer: Не удалось выделить VkDeviceMemory под буфер!");
    }

    // Шаг 4: Жестко связываем виртуальный буфер и физическую память GPU
    // Смещение (offset) равен 0, так как мы выделяем отдельный кусок памяти под один буфер
    vkBindBufferMemory(LogicalDeviceRef, BufferHandle, MemoryHandle, 0);
}

FErolyssaBuffer::~FErolyssaBuffer()
{
    Unmap();

    if (BufferHandle != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(LogicalDeviceRef, BufferHandle, nullptr);
    }
    if (MemoryHandle != VK_NULL_HANDLE)
    {
        vkFreeMemory(LogicalDeviceRef, MemoryHandle, nullptr);
    }
}

void* FErolyssaBuffer::Map()
{
    if(MappedPointer) return MappedPointer;
    
    if(vkMapMemory(LogicalDeviceRef, MemoryHandle, 0, Size, 0, &MappedPointer) != VK_SUCCESS)
        ErolyssaTerminate("FErolyssaBuffer: Не удалось замапить память буфера!");
    
    return MappedPointer;
}

void FErolyssaBuffer::Unmap()
{
    if(MappedPointer == nullptr) return;
    
    vkUnmapMemory(LogicalDeviceRef, MemoryHandle);
    MappedPointer = nullptr;
}

bool FErolyssaBuffer::CopyFrom(const void* InData, const int32_t InSize)
{
    if(!InData || InSize == 0) return false;
    
    void* MappedData = Map();
    if(!MappedData) return false;
    
    memcpy(MappedData, InData, InSize);
    Unmap();
    return true;
}

uint32_t FErolyssaBuffer::FindMemoryType(VkPhysicalDevice PhysicalDevice, uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
{
    // Опрашиваем физическое устройство обо всех доступных типах памяти на этом железе
    VkPhysicalDeviceMemoryProperties MemProperties;
    vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemProperties);

    for (uint32_t i = 0; i < MemProperties.memoryTypeCount; ++i)
    {
        // 1. Бит TypeFilter показывает, какие типы памяти совместимы с нашим VkBuffer (каждый бит — индекс типа)
        // 2. Проверяем, что данный тип памяти обладает всеми нужными нам флагами (например, HOST_VISIBLE)
        if ((TypeFilter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
        {
            return i;
        }
    }

    ErolyssaTerminate("FErolyssaBuffer: Не удалось найти подходящий тип памяти на GPU!");
    return 0xFFFFFFFF;
}
