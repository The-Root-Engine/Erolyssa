// Root Engine / Erolyssa

#include "ErolyssaPhysicalDevice.h"
#include "ErolyssaInstance.h"
#include <cstdio>
#include <cstdlib>

#include "Erolyssa.h"

FErolyssaPhysicalDevice::FErolyssaPhysicalDevice(const FErolyssaInstance& InInstance)
{
    PickPhysicalDevice(InInstance.GetHandle());
    CreateLogicalDevice();
}

FErolyssaPhysicalDevice::~FErolyssaPhysicalDevice()
{
    if (DeviceHandle != VK_NULL_HANDLE)
    {
        vkDestroyDevice(DeviceHandle, nullptr);
    }
}

void FErolyssaPhysicalDevice::PickPhysicalDevice(VkInstance InstanceHandle)
{
    uint32_t DeviceCount = 0;
    vkEnumeratePhysicalDevices(InstanceHandle, &DeviceCount, nullptr);

    if (DeviceCount == 0)
    {
        ErolyssaTerminate("FErolyssaDevice: Не найдено ни одной видеокарты с поддержкой Vulkan!");
    }

    std::vector<VkPhysicalDevice> Devices(DeviceCount);
    vkEnumeratePhysicalDevices(InstanceHandle, &DeviceCount, Devices.data());

    int HighestScore = -1;
    VkPhysicalDevice BestDevice = VK_NULL_HANDLE;
    
    for (const auto& Device : Devices)
    {
        int Score = RateDeviceSuitability(Device);
        if (Score > HighestScore)
        {
            HighestScore = Score;
            BestDevice = Device;
        }
    }
    
    if (BestDevice == VK_NULL_HANDLE || HighestScore < 0)
    {
        ErolyssaTerminate("FErolyssaDevice: В системе нет GPU, подходящего под требования Compute!");
    }
    
    PhysicalDeviceHandle = BestDevice;
    
    VkPhysicalDeviceProperties DeviceProperties;
    vkGetPhysicalDeviceProperties(PhysicalDeviceHandle, &DeviceProperties);
    printf("[Erolyssa Device]: Selected GPU: %s\n", DeviceProperties.deviceName);
}

int FErolyssaPhysicalDevice::RateDeviceSuitability(VkPhysicalDevice Device)
{
    VkPhysicalDeviceProperties DeviceProperties;
    VkPhysicalDeviceFeatures DeviceFeatures;
    vkGetPhysicalDeviceProperties(Device, &DeviceProperties);
    vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);

    // Если на GPU вообще нельзя запустить вычисления, он нам не подходит
    uint32_t QueueIndex = FindComputeQueueFamily(Device);
    if (QueueIndex == 0xFFFFFFFF) return -1;

    int Score = 0;

    // Дискретная видеокарта всегда в приоритете перед встроенной
    if (DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        Score += 1000;
    }
    else if (DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    {
        Score += 100;
    }

    return Score;
}

uint32_t FErolyssaPhysicalDevice::FindComputeQueueFamily(VkPhysicalDevice Device)
{
    uint32_t QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

    uint32_t UniversalComputeIndex = 0xFFFFFFFF;

    // Ищем очереди
    for (uint32_t i = 0; i < QueueFamilyCount; ++i)
    {
        // Проверяем наличие флага вычислений
        if (QueueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            // Идеальный вариант: Async/Dedicated Compute (есть вычисления, но нет графики)
            if ((QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
            {
                return i; 
            }
            
            // Запасной вариант: Универсальная очередь (Compute + Graphics)
            if (UniversalComputeIndex == 0xFFFFFFFF)
            {
                UniversalComputeIndex = i;
            }
        }
    }

    return UniversalComputeIndex;
}

void FErolyssaPhysicalDevice::CreateLogicalDevice()
{
    // Находим индекс нужной нам очереди еще раз для физического устройства
    ComputeQueueFamilyIndex = FindComputeQueueFamily(PhysicalDeviceHandle);
    
    // Настройка создания очереди. Приоритет от 0.0 до 1.0 (важность планирования потоков драйвером)
    float QueuePriority = 1.0f;
    VkDeviceQueueCreateInfo QueueCreateInfo{};
    QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueCreateInfo.queueFamilyIndex = ComputeQueueFamilyIndex;
    QueueCreateInfo.queueCount = 1;
    QueueCreateInfo.pQueuePriorities = &QueuePriority;

    // Запрашиваем фичи устройства. Пока оставляем базовые.
    VkPhysicalDeviceFeatures DeviceFeatures{};

    VkDeviceCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    CreateInfo.queueCreateInfoCount = 1;
    CreateInfo.pQueueCreateInfos = &QueueCreateInfo;
    CreateInfo.pEnabledFeatures = &DeviceFeatures;

    // В Vulkan 1.3 слои устройства (Device Layers) устарели (они берутся из инстанса), 
    // поэтому выставляем их в ноль. Разрешения на расширения нам пока тоже не нужны.
    CreateInfo.enabledExtensionCount = 0;
    CreateInfo.enabledLayerCount = 0;

    if (vkCreateDevice(PhysicalDeviceHandle, &CreateInfo, nullptr, &DeviceHandle) != VK_SUCCESS)
    {
        ErolyssaTerminate("FErolyssaDevice: Не удалось создать логическое устройство VkDevice!");
    }

    // Извлекаем дескриптор очереди из логического устройства
    vkGetDeviceQueue(DeviceHandle, ComputeQueueFamilyIndex, 0, &ComputeQueueHandle);
}
