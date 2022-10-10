#pragma once

#include <vulkan/vulkan.hpp>
#include <unordered_map>

#include "Window.hpp"
#include "vk/LogicalDevice.hpp"
#include "vk/PhysicalDevice.hpp"

namespace ve
{
    struct VulkanMainContext {
        VulkanMainContext(const uint32_t width, const uint32_t height) : window(width, height), instance(window.get_required_extensions()), surface(window.create_surface(instance.get())), physical_device(instance, surface), logical_device(physical_device, queues_family_indices, queues)
        {
            VE_LOG_CONSOLE(VE_INFO, VE_C_PINK << "Created VulkanMainContext\n");
        }
        
        void self_destruct()
        {
            instance.get().destroySurfaceKHR(surface);
            logical_device.self_destruct();
            instance.self_destruct();
            window.self_destruct();
            VE_LOG_CONSOLE(VE_INFO, VE_C_PINK << "Destroyed VulkanMainContext\n");
        }

        std::vector<vk::SurfaceFormatKHR> get_surface_formats() const
        {
            return physical_device.get().getSurfaceFormatsKHR(surface);
        }

        std::vector<vk::PresentModeKHR> get_surface_present_modes() const
        {
            return physical_device.get().getSurfacePresentModesKHR(surface);
        }

        vk::SurfaceCapabilitiesKHR get_surface_capabilities() const
        {
            return physical_device.get().getSurfaceCapabilitiesKHR(surface);
        }

        const vk::Queue& get_graphics_queue() const
        {
            return queues.at(QueueIndex::Graphics);
        }

        const vk::Queue& get_transfer_queue() const
        {
            return queues.at(QueueIndex::Transfer);
        }

        const vk::Queue& get_compute_queue() const
        {
            return queues.at(QueueIndex::Compute);
        }

        const vk::Queue& get_present_queue() const
        {
            return queues.at(QueueIndex::Present);
        }

    private:
        std::unordered_map<QueueIndex, vk::Queue> queues;

    public:
        QueueFamilyIndices queues_family_indices;
        Window window;
        Instance instance;
        vk::SurfaceKHR surface;
        PhysicalDevice physical_device;
        LogicalDevice logical_device;
    };
}// namespace ve