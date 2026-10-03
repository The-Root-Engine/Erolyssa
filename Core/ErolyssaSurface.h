// Root Engine / Erolyssa

#pragma once

#include "../ErolyssaIncludeVulkan.h"
#include "../ErolyssaTatemae.h"
#include "../ErolyssaAliases.h"

#include <SDL_video.h>
#include <SDL_vulkan.h>

class FErolyssaSurface
{
public:
    FErolyssaSurface(const FErolyssaInstance& InInstance, SDL_Window* InWindow) : Instance(InInstance)
    {
        check(InWindow != nullptr, "FErolyssaSurface: Window is null!");
        check(SDL_Vulkan_CreateSurface(InWindow, Instance, &Handle) == SDL_TRUE, "FErolyssaSurface: SDL_Vulkan_CreateSurface failed: %s", SDL_GetError());
    }
    
    ~FErolyssaSurface()
    {
        if(Handle != VK_NULL_HANDLE) vkDestroySurfaceKHR(Instance, Handle, nullptr);
    }
    
    FErolyssaSurface(const FErolyssaSurface&) = delete;
    FErolyssaSurface& operator=(const FErolyssaSurface&) = delete;
    
    operator VkSurfaceKHR() const { return Handle; }

private:
    const FErolyssaInstance& Instance;
    VkSurfaceKHR Handle = VK_NULL_HANDLE;
};
