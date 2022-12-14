#include <iostream>
#include <stdexcept>
#include <thread>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "Camera.hpp"
#include "EventHandler.hpp"
#include "ve_log.hpp"
#include "vk/VulkanCommandContext.hpp"
#include "vk/VulkanMainContext.hpp"
#include "vk/VulkanRenderContext.hpp"

struct RenderingInfo {
    RenderingInfo(uint32_t width, uint32_t height) : width(width), height(height)
    {}
    uint32_t width;
    uint32_t height;
};

class MainContext
{
public:
    MainContext(const RenderingInfo& ri) : vmc(ri.width, ri.height), vcc(vmc), vrc(vmc, vcc), camera(45.0f, ri.width, ri.height)
    {
        vk::Extent2D extent = vrc.swapchain.get_extent();
        camera.updateScreenSize(extent.width, extent.height);
    }

    ~MainContext()
    {
        vcc.self_destruct();
        vrc.self_destruct();
        vmc.self_destruct();
        VE_LOG_CONSOLE(VE_INFO, VE_C_PINK << "Destroyed MainContext" << std::endl);
    }

    void run()
    {
        constexpr double min_frametime = 5.0;
        auto t1 = std::chrono::high_resolution_clock::now();
        auto t2 = std::chrono::high_resolution_clock::now();
        // keep time measurement and frametime separate to be able to use a frame limiter
        double duration = 0.0;
        double frametime = 0.0;
        bool quit = false;
        SDL_Event e;
        while (!quit)
        {
            move_amount = duration * move_speed;
            dispatch_pressed_keys();
            try
            {
                std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(min_frametime - frametime));
                vrc.draw_frame(camera, duration / 1000.0f);
            }
            catch (const vk::OutOfDateKHRError e)
            {
                vk::Extent2D extent = vrc.recreate_swapchain();
                camera.updateScreenSize(extent.width, extent.height);
            }
            while (SDL_PollEvent(&e))
            {
                quit = e.window.event == SDL_WINDOWEVENT_CLOSE;
                eh.dispatch_event(e);
            }
            t2 = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration<double, std::milli>(t2 - t1).count();
            // calculate actual frametime by subtracting the waiting time
            frametime = duration - std::max(0.0, min_frametime - frametime);
            vmc.window->set_title(ve::to_string(duration, 4) + " ms; FPS: " + ve::to_string(1000.0 / duration) + " (" + ve::to_string(frametime, 4) + " ms; FPS: " + ve::to_string(1000.0 / frametime) + ")");
            t1 = t2;
        }
    }

private:
    ve::VulkanMainContext vmc;
    ve::VulkanCommandContext vcc;
    ve::VulkanRenderContext vrc;
    Camera camera;
    EventHandler eh;
    float move_amount;
    float move_speed = 0.02f;

    void dispatch_pressed_keys()
    {
        if (eh.pressed_keys.contains(Key::W)) camera.moveFront(move_amount);
        if (eh.pressed_keys.contains(Key::A)) camera.moveRight(-move_amount);
        if (eh.pressed_keys.contains(Key::S)) camera.moveFront(-move_amount);
        if (eh.pressed_keys.contains(Key::D)) camera.moveRight(move_amount);
        if (eh.pressed_keys.contains(Key::Q)) camera.moveDown(move_amount);
        if (eh.pressed_keys.contains(Key::E)) camera.moveDown(-move_amount);

        // reset state of keys that are used to execute a one time action
        if (eh.pressed_keys.contains(Key::Plus))
        {
            move_speed += 0.01f;
            eh.pressed_keys.erase(Key::Plus);
        }
        if (eh.pressed_keys.contains(Key::Minus))
        {
            move_speed -= 0.01f;
            eh.pressed_keys.erase(Key::Minus);
        }
        if (eh.pressed_keys.contains(Key::MouseLeft))
        {
            if (!SDL_GetRelativeMouseMode()) SDL_SetRelativeMouseMode(SDL_TRUE);
            camera.onMouseMove(eh.mouse_motion.x * 1.5f, eh.mouse_motion.y * 1.5f);
            eh.mouse_motion = glm::vec2(0.0f);
        }
        if (eh.released_keys.contains(Key::MouseLeft))
        {
            if (SDL_GetRelativeMouseMode()) SDL_SetRelativeMouseMode(SDL_FALSE);
            eh.released_keys.erase(Key::MouseLeft);
        }
    }
};

int main(int argc, char** argv)
{
    VE_LOG_CONSOLE(VE_INFO, "Starting\n");
    auto t1 = std::chrono::high_resolution_clock::now();
    RenderingInfo ri(1000, 800);
    MainContext mc(ri);
    auto t2 = std::chrono::high_resolution_clock::now();
    VE_LOG_CONSOLE(VE_INFO, VE_C_BLUE << "Setup took: " << (std::chrono::duration<double, std::milli>(t2 - t1).count()) << "ms" << std::endl);
    mc.run();
    return 0;
}
