#pragma once

#include <unordered_map>
#include <vulkan/vulkan.hpp>

#include "vk/PhysicalDevice.hpp"

namespace ve
{
    enum class QueueIndex
    {
        Graphics,
        Compute,
        Transfer,
        Present
    };

    class LogicalDevice
    {
    public:
        LogicalDevice(const PhysicalDevice& p_device, QueueFamilyIndices& indices, std::unordered_map<QueueIndex, vk::Queue>& queues);
        void self_destruct();
        const vk::Device& get() const;

    private:
        vk::Device device;
    };
}// namespace ve
