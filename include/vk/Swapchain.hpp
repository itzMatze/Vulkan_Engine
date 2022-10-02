#pragma once

#include <vulkan/vulkan.hpp>

#include "ve_log.hpp"
#include "vk/PhysicalDevice.hpp"

namespace ve
{
    class Swapchain
    {
    public:
        Swapchain(const PhysicalDevice& physical_device, const vk::Device device, const vk::SurfaceKHR surface, SDL_Window* window)
        {
            VE_LOG_CONSOLE(PINK << "Creating swapchain");
            std::vector<vk::SurfaceFormatKHR> formats = physical_device.get().getSurfaceFormatsKHR(surface);
            vk::SurfaceFormatKHR surface_format = choose_surface_format(formats);
            format = surface_format.format;
            std::vector<vk::PresentModeKHR> present_modes = physical_device.get().getSurfacePresentModesKHR(surface);
            vk::SurfaceCapabilitiesKHR capabilities = physical_device.get().getSurfaceCapabilitiesKHR(surface);
            extent = choose_extent(capabilities, window);
            uint32_t image_count = capabilities.maxImageCount > 0 ? std::min(capabilities.minImageCount + 1, capabilities.maxImageCount) : capabilities.minImageCount + 1;
            vk::SwapchainCreateInfoKHR sci{};
            sci.sType = vk::StructureType::eSwapchainCreateInfoKHR;
            sci.surface = surface;
            sci.minImageCount = image_count;
            sci.imageFormat = surface_format.format;
            sci.imageColorSpace = surface_format.colorSpace;
            sci.imageExtent = extent;
            sci.imageArrayLayers = 1;
            sci.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
            sci.preTransform = capabilities.currentTransform;
            sci.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
            sci.presentMode = choose_present_mode(present_modes);
            sci.clipped = VK_TRUE;
            sci.oldSwapchain = VK_NULL_HANDLE;
            QueueFamilyIndices indices = physical_device.get_queue_families();
            uint32_t queue_family_indices[] = {static_cast<uint32_t>(indices.graphics), static_cast<uint32_t>(indices.present)};
            if (indices.graphics != indices.present)
            {
                VE_LOG_CONSOLE("Graphics and Presentation queue are two distinct queues. Using Concurrent sharing mode on swapchain.");
                sci.imageSharingMode = vk::SharingMode::eConcurrent;
                sci.queueFamilyIndexCount = 2;
                sci.pQueueFamilyIndices = queue_family_indices;
            }
            else
            {
                VE_LOG_CONSOLE("Graphics and Presentation queue are the same queue. Using Exclusive sharing mode on swapchain.");
                sci.imageSharingMode = vk::SharingMode::eExclusive;
            }
            swapchain = device.createSwapchainKHR(sci);
            swapchain_images = device.getSwapchainImagesKHR(swapchain);
        }

        vk::SwapchainKHR get()
        {
            return swapchain;
        }

    private:
        vk::SurfaceFormatKHR choose_surface_format(const std::vector<vk::SurfaceFormatKHR>& available_formats)
        {
            for (const auto& format: available_formats)
            {
                if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) return format;
            }
            VE_WARN_CONSOLE("Desired format not found. Using first available.");
            return available_formats[0];
        }

        vk::PresentModeKHR choose_present_mode(const std::vector<vk::PresentModeKHR>& available_present_modes)
        {
            for (const auto& pm: available_present_modes)
            {
                if (pm == vk::PresentModeKHR::eImmediate) return pm;
            }
            VE_WARN_CONSOLE("Desired present mode not found. Using FIFO.");
            return vk::PresentModeKHR::eFifo;
        }

        vk::Extent2D choose_extent(const vk::SurfaceCapabilitiesKHR& capabilities, SDL_Window* window)
        {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            {
                return capabilities.currentExtent;
            }
            else
            {
                int32_t width, height;
                SDL_Vulkan_GetDrawableSize(window, &width, &height);
                vk::Extent2D extent(width, height);
                extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
                return extent;
            }
        }
        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> swapchain_images;
        vk::Format format;
        vk::Extent2D extent;
    };
}// namespace ve