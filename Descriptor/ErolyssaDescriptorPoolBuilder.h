#pragma once

#include <vector>
#include "ErolyssaBuffer.h"

struct FErolyssaDescriptorSetBuilder;
class FErolyssaDescriptorSet;
struct FErolyssaDescriptorPool;
class FErolyssaPhysicalDevice;

struct FErolyssaDescriptorPoolBuilder
{
    
public:
    FErolyssaDescriptorPoolBuilder(FErolyssaPhysicalDevice& InDevice, FErolyssaDescriptorPool& OutPool);
    
    FErolyssaDescriptorPoolBuilder& BindSet(FErolyssaDescriptorSetBuilder SetBuilder);
    
    void Build();

private:
    FErolyssaPhysicalDevice& Device;
    FErolyssaDescriptorPool& Pool;
    
    std::vector<FErolyssaDescriptorSetBuilder> SetBuilders;
};
