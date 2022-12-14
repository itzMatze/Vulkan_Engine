#include "vk/VulkanMainContext.hpp"

#include "ve_log.hpp"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace ve
{
    // create VulkanMainContext without window for non graphical applications
    VulkanMainContext::VulkanMainContext() : instance({}), physical_device(instance, surface), logical_device(physical_device, queues_family_indices, queues)
    {
        create_vma_allocator();
        VE_LOG_CONSOLE(VE_INFO, VE_C_PINK << "Created VulkanMainContext\n");
    }

    // create VulkanMainContext with window for graphical applications
    VulkanMainContext::VulkanMainContext(const uint32_t width, const uint32_t height) : window(std::make_optional<Window>(width, height)), instance(window->get_required_extensions()), surface(window->create_surface(instance.get())), physical_device(instance, surface), logical_device(physical_device, queues_family_indices, queues)
    {
        create_vma_allocator();
        VE_LOG_CONSOLE(VE_INFO, VE_C_PINK << "Created VulkanMainContext\n");
    }

    void VulkanMainContext::self_destruct()
    {
        vmaDestroyAllocator(va);
        if (surface.has_value()) instance.get().destroySurfaceKHR(surface.value());
        logical_device.self_destruct();
        instance.self_destruct();
        if (window.has_value()) window->self_destruct();
        VE_LOG_CONSOLE(VE_INFO, VE_C_PINK << "Destroyed VulkanMainContext\n");
    }

    std::vector<vk::SurfaceFormatKHR> VulkanMainContext::get_surface_formats() const
    {
        if (!surface.has_value()) VE_THROW("Surface not initialized!");
        return physical_device.get().getSurfaceFormatsKHR(surface.value());
    }

    std::vector<vk::PresentModeKHR> VulkanMainContext::get_surface_present_modes() const
    {
        if (!surface.has_value()) VE_THROW("Surface not initialized!");
        return physical_device.get().getSurfacePresentModesKHR(surface.value());
    }

    vk::SurfaceCapabilitiesKHR VulkanMainContext::get_surface_capabilities() const
    {
        if (!surface.has_value()) VE_THROW("Surface not initialized!");
        return physical_device.get().getSurfaceCapabilitiesKHR(surface.value());
    }

    const vk::Queue& VulkanMainContext::get_graphics_queue() const
    {
        return queues.at(QueueIndex::Graphics);
    }

    const vk::Queue& VulkanMainContext::get_transfer_queue() const
    {
        return queues.at(QueueIndex::Transfer);
    }

    const vk::Queue& VulkanMainContext::get_compute_queue() const
    {
        return queues.at(QueueIndex::Compute);
    }

    const vk::Queue& VulkanMainContext::get_present_queue() const
    {
        return queues.at(QueueIndex::Present);
    }

    void VulkanMainContext::create_vma_allocator()
    {
        VmaAllocatorCreateInfo vaci{};
        vaci.instance = instance.get();
        vaci.physicalDevice = physical_device.get();
        vaci.device = logical_device.get();
        vaci.vulkanApiVersion = VK_API_VERSION_1_3;
        vmaCreateAllocator(&vaci, &va);
    }
}// namespace ve