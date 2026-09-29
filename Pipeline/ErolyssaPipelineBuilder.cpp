// Root Engine / Erolyssa

#include "ErolyssaPipelineBuilder.h"

#include "Erolyssa.h"
#include "ErolyssaPhysicalDevice.h"
#include "ErolyssaPipeline.h"

void FErolyssaPipelineBuilder::Build()
{
    if (!ShaderPath)
        ErolyssaTerminate("FErolyssaPipelineBuilder: Shader file path is Empty!");

    const VkDevice LogicalDevice = Device.GetLogicalHandle();
    const auto ShaderCode = ReadShaderFile(ShaderPath);
    VkShaderModule ShaderModuleHandle = VK_NULL_HANDLE;
    
    {
        VkShaderModuleCreateInfo ModuleCreateInfo{};
        ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        ModuleCreateInfo.codeSize = ShaderCode.size();
        ModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(ShaderCode.data());
    
        if(vkCreateShaderModule(LogicalDevice, &ModuleCreateInfo, nullptr, &ShaderModuleHandle) != VK_SUCCESS)
            ErolyssaTerminate("FErolyssaPipelineBuilder: Failed to create VkShaderModule from SPIR-V file!");
    }
    
    {
        std::vector<VkDescriptorSetLayoutBinding> Bindings;
        for(uint32_t i = 0; i < BindingTypes.size(); ++i)
        {
            VkDescriptorSetLayoutBinding Binding{};
            Binding.binding = i;
            Binding.descriptorType = BindingTypes[i];
            Binding.descriptorCount = 1;
            Binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            Binding.pImmutableSamplers = nullptr;
            Bindings.push_back(Binding);
        }
    
        VkDescriptorSetLayoutCreateInfo LayoutCreateInfo{};
        LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        LayoutCreateInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
        LayoutCreateInfo.pBindings = Bindings.data();
    
        if(vkCreateDescriptorSetLayout(LogicalDevice, &LayoutCreateInfo, nullptr, &Pipeline.DescriptorSetLayoutHandle) != VK_SUCCESS)
            ErolyssaTerminate("FErolyssaPipelineBuilder: Failed to create VkDescriptorSetLayout!");
    }
    
    {
        VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo{};
        PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutCreateInfo.setLayoutCount = 1;
        PipelineLayoutCreateInfo.pSetLayouts = &Pipeline.DescriptorSetLayoutHandle;
    
        VkPushConstantRange PushConstantRange{};
        if(PushConstantSize > 0)
        {
            PushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            PushConstantRange.offset = 0;
            PushConstantRange.size = PushConstantSize;

            PipelineLayoutCreateInfo.pushConstantRangeCount = 1;
            PipelineLayoutCreateInfo.pPushConstantRanges = &PushConstantRange;
        }
        else
        {
            PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
            PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        }
    
        if(vkCreatePipelineLayout(LogicalDevice, &PipelineLayoutCreateInfo, nullptr, &Pipeline.PipelineLayoutHandle) != VK_SUCCESS)
            ErolyssaTerminate("FErolyssaPipelineBuilder: Failed to create VkPipelineLayout!");
    }
    
    {
        VkComputePipelineCreateInfo ComputePipelineCreateInfo{};
        ComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        ComputePipelineCreateInfo.layout = Pipeline.PipelineLayoutHandle;
        
        ComputePipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ComputePipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        ComputePipelineCreateInfo.stage.module = ShaderModuleHandle;
        ComputePipelineCreateInfo.stage.pName = "main";
        
        if(vkCreateComputePipelines(LogicalDevice, VK_NULL_HANDLE, 1, &ComputePipelineCreateInfo, nullptr, &Pipeline.PipelineHandle) != VK_SUCCESS)
            ErolyssaTerminate("FErolyssaPipelineBuilder: Failed to compile VkPipeline!");
    }
    
    vkDestroyShaderModule(LogicalDevice, ShaderModuleHandle, nullptr);
}

std::vector<char> FErolyssaPipelineBuilder::ReadShaderFile(const char* FilePath)
{
    FILE* File = fopen(FilePath, "rb");
    if(!File)
        ErolyssaTerminate("FErolyssaPipeline: Критическая ошибка! Не удалось открыть файл шейдера.");
    
    fseek(File, 0, SEEK_END);
    long FileSize = ftell(File);
    rewind(File);
    
    std::vector<char> Buffer(FileSize);
    size_t BytesRead = fread(Buffer.data(), 1, FileSize, File);
    fclose(File);
    
    if(BytesRead != static_cast<size_t>(FileSize))
        ErolyssaTerminate("FErolyssaPipeline: Не удалось полностью прочитать файл шейдера.");
    
    return Buffer;
}
